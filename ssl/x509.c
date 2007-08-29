/*
 *  Copyright(C) 2006 Cameron Rich
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file x509.c
 * 
 * Certificate processing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "os_port.h"
#include "crypto.h"
#include "crypto_misc.h"

#ifdef CONFIG_SSL_CERT_VERIFICATION
/**
 * Retrieve the signature from a certificate.
 */
const uint8_t *x509_get_signature(const uint8_t *asn1_sig, int *len)
{
    int offset = 0;
    const uint8_t *ptr = NULL;

    if (asn1_next_obj(asn1_sig, &offset, ASN1_SEQUENCE) < 0 || 
            asn1_skip_obj(asn1_sig, &offset, ASN1_SEQUENCE))
        goto end_get_sig;

    if (asn1_sig[offset++] != ASN1_OCTET_STRING)
        goto end_get_sig;
    *len = get_asn1_length(asn1_sig, &offset);
    ptr = &asn1_sig[offset];          /* all ok */

end_get_sig:
    return ptr;
}

#endif

/**
 * Construct a new x509 object.
 * @return 0 if ok. < 0 if there was a problem.
 */
int x509_new(const uint8_t *cert, int *len, X509_CTX **ctx)
{
    int begin_tbs, end_tbs;
    int ret = X509_NOT_OK, offset = 0, cert_size = 0;
    X509_CTX *x509_ctx;
    BI_CTX *bi_ctx;

    *ctx = (X509_CTX *)calloc(1, sizeof(X509_CTX));
    x509_ctx = *ctx;

    /* get the certificate size */
    asn1_skip_obj(cert, &cert_size, ASN1_SEQUENCE); 

    if (asn1_next_obj(cert, &offset, ASN1_SEQUENCE) < 0)
        goto end_cert;

    begin_tbs = offset;         /* start of the tbs */
    end_tbs = begin_tbs;        /* work out the end of the tbs */
    asn1_skip_obj(cert, &end_tbs, ASN1_SEQUENCE);

    if (asn1_next_obj(cert, &offset, ASN1_SEQUENCE) < 0)
        goto end_cert;

    if (cert[offset] == ASN1_EXPLICIT_TAG)   /* optional version */
    {
        if (asn1_version(cert, &offset, x509_ctx))
            goto end_cert;
    }

    if (asn1_skip_obj(cert, &offset, ASN1_INTEGER) || /* serial number */ 
            asn1_next_obj(cert, &offset, ASN1_SEQUENCE) < 0)
        goto end_cert;

    /* make sure the signature is ok */
    if (asn1_signature_type(cert, &offset, x509_ctx))
    {
        ret = X509_VFY_ERROR_UNSUPPORTED_DIGEST;
        goto end_cert;
    }

    if (asn1_name(cert, &offset, x509_ctx->ca_cert_dn) || 
            asn1_validity(cert, &offset, x509_ctx) ||
            asn1_name(cert, &offset, x509_ctx->cert_dn) ||
            asn1_public_key(cert, &offset, x509_ctx))
        goto end_cert;

    bi_ctx = x509_ctx->rsa_ctx->bi_ctx;

#ifdef CONFIG_SSL_CERT_VERIFICATION /* only care if doing verification */
    /* use the appropriate signature algorithm (either SHA1 or MD5) */
    if (x509_ctx->sig_type == SIG_TYPE_MD5)
    {
        MD5_CTX md5_ctx;
        uint8_t md5_dgst[MD5_SIZE];
        MD5_Init(&md5_ctx);
        MD5_Update(&md5_ctx, &cert[begin_tbs], end_tbs-begin_tbs);
        MD5_Final(md5_dgst, &md5_ctx);
        x509_ctx->digest = bi_import(bi_ctx, md5_dgst, MD5_SIZE);
    }
    else if (x509_ctx->sig_type == SIG_TYPE_SHA1)
    {
        SHA1_CTX sha_ctx;
        uint8_t sha_dgst[SHA1_SIZE];
        SHA1_Init(&sha_ctx);
        SHA1_Update(&sha_ctx, &cert[begin_tbs], end_tbs-begin_tbs);
        SHA1_Final(sha_dgst, &sha_ctx);
        x509_ctx->digest = bi_import(bi_ctx, sha_dgst, SHA1_SIZE);
    }

    offset = end_tbs;   /* skip the v3 data */
    if (asn1_skip_obj(cert, &offset, ASN1_SEQUENCE) || 
            asn1_signature(cert, &offset, x509_ctx))
        goto end_cert;
#endif

    if (len)
    {
        *len = cert_size;
    }

    ret = X509_OK;
end_cert:

#ifdef CONFIG_SSL_FULL_MODE
    if (ret)
    {
        printf("Error: Invalid X509 ASN.1 file\n");
    }
#endif

    return ret;
}

/**
 * Free an X.509 object's resources.
 */
void x509_free(X509_CTX *x509_ctx)
{
    X509_CTX *next;
    int i;

    if (x509_ctx == NULL)       /* if already null, then don't bother */
        return;

    for (i = 0; i < X509_NUM_DN_TYPES; i++)
    {
        free(x509_ctx->ca_cert_dn[i]);
        free(x509_ctx->cert_dn[i]);
    }

    free(x509_ctx->signature);

#ifdef CONFIG_SSL_CERT_VERIFICATION 
    if (x509_ctx->digest)
    {
        bi_free(x509_ctx->rsa_ctx->bi_ctx, x509_ctx->digest);
    }
#endif

    RSA_free(x509_ctx->rsa_ctx);

    next = x509_ctx->next;
    free(x509_ctx);
    x509_free(next);        /* clear the chain */
}

#ifdef CONFIG_SSL_CERT_VERIFICATION
/**
 * Do some basic checks on the certificate chain.
 *
 * Certificate verification consists of a number of checks:
 * - A root certificate exists in the certificate store.
 * - The date of the certificate is after the start date.
 * - The date of the certificate is before the finish date.
 * - The certificate chain is valid.
 * - That the certificate(s) are not self-signed.
 * - The signature of the certificate is valid.
 */
int x509_verify(const CA_CERT_CTX *ca_cert_ctx, const X509_CTX *cert) 
{
    int ret = X509_OK, i = 0;
    bigint *cert_sig;
    X509_CTX *next_cert = NULL;
    BI_CTX *ctx;
    bigint *mod, *expn;
    struct timeval tv;
    int match_ca_cert = 0;

    if (cert == NULL || ca_cert_ctx == NULL)
    {
        ret = X509_VFY_ERROR_NO_TRUSTED_CERT;       
        goto end_verify;
    }

    /* last cert in the chain - look for a trusted cert */
    if (cert->next == NULL)
    {
        while (i < CONFIG_X509_MAX_CA_CERTS && ca_cert_ctx->cert[i])
        {
            if (asn1_compare_dn(cert->ca_cert_dn,
                                        ca_cert_ctx->cert[i]->cert_dn) == 0)
            {
                match_ca_cert = 1;
                break;
            }

            i++;
        }

        if (i < CONFIG_X509_MAX_CA_CERTS && ca_cert_ctx->cert[i])
        {
            next_cert = ca_cert_ctx->cert[i];
        }
        else    /* trusted cert not found */
        {
            ret = X509_VFY_ERROR_NO_TRUSTED_CERT;       
            goto end_verify;
        }
    }
    else
    {
        next_cert = cert->next;
    }

    gettimeofday(&tv, NULL);

    /* check the not before date */
    if (tv.tv_sec < cert->not_before)
    {
        ret = X509_VFY_ERROR_NOT_YET_VALID;
        goto end_verify;
    }

    /* check the not after date */
    if (tv.tv_sec > cert->not_after)
    {
        ret = X509_VFY_ERROR_EXPIRED;
        goto end_verify;
    }

    /* check the chain integrity */
    if (asn1_compare_dn(cert->ca_cert_dn, next_cert->cert_dn))
    {
        ret = X509_VFY_ERROR_INVALID_CHAIN;
        goto end_verify;
    }

    /* check for self-signing */
    if (!match_ca_cert && asn1_compare_dn(cert->ca_cert_dn, cert->cert_dn) == 0)
    {
        ret = X509_VFY_ERROR_SELF_SIGNED;
        goto end_verify;
    }

    /* check the signature */
    ctx = cert->rsa_ctx->bi_ctx;
    mod = next_cert->rsa_ctx->m;
    expn = next_cert->rsa_ctx->e;
    cert_sig = RSA_sign_verify(ctx, cert->signature, cert->sig_len, 
            bi_clone(ctx, mod), bi_clone(ctx, expn));

    if (cert_sig)
    {
        ret = cert->digest ?    /* check the signature */
            bi_compare(cert_sig, cert->digest) :
            X509_VFY_ERROR_UNSUPPORTED_DIGEST;
        bi_free(ctx, cert_sig);

        if (ret)
            goto end_verify;
    }
    else
    {
        ret = X509_VFY_ERROR_BAD_SIGNATURE;
        goto end_verify;
    }

    /* go down the certificate chain using recursion. */
    if (ret == 0 && cert->next)
    {
        ret = x509_verify(ca_cert_ctx, next_cert);
    }

end_verify:
    return ret;
}
#endif

#if defined (CONFIG_SSL_FULL_MODE)
/**
 * Used for diagnostics.
 */
void x509_print(CA_CERT_CTX *ca_cert_ctx, const X509_CTX *cert) 
{
    if (cert == NULL)
        return;

    printf("----------------   CERT DEBUG   ----------------\n");
    printf("* CA Cert Distinguished Name\n");
    if (cert->ca_cert_dn[X509_COMMON_NAME])
    {
        printf("Common Name (CN):\t%s\n", cert->ca_cert_dn[X509_COMMON_NAME]);
    }

    if (cert->ca_cert_dn[X509_ORGANIZATION])
    {
        printf("Organization (O):\t%s\n", cert->ca_cert_dn[X509_ORGANIZATION]);
    }

    if (cert->ca_cert_dn[X509_ORGANIZATIONAL_TYPE])
    {
        printf("Organizational Unit (OU): %s\n", 
                cert->ca_cert_dn[X509_ORGANIZATIONAL_TYPE]);
    }

    printf("* Cert Distinguished Name\n");
    if (cert->cert_dn[X509_COMMON_NAME])
    {
        printf("Common Name (CN):\t%s\n", cert->cert_dn[X509_COMMON_NAME]);
    }

    if (cert->cert_dn[X509_ORGANIZATION])
    {
        printf("Organization (O):\t%s\n", cert->cert_dn[X509_ORGANIZATION]);
    }

    if (cert->cert_dn[X509_ORGANIZATIONAL_TYPE])
    {
        printf("Organizational Unit (OU): %s\n", 
                cert->cert_dn[X509_ORGANIZATIONAL_TYPE]);
    }

    printf("Not Before:\t\t%s", ctime(&cert->not_before));
    printf("Not After:\t\t%s", ctime(&cert->not_after));
    printf("RSA bitsize:\t\t%d\n", cert->rsa_ctx->num_octets*8);
    printf("Sig Type:\t\t");
    switch (cert->sig_type)
    {
        case SIG_TYPE_MD5:
            printf("MD5\n");
            break;
        case SIG_TYPE_SHA1:
            printf("SHA1\n");
            break;
        case SIG_TYPE_MD2:
            printf("MD2\n");
            break;
        default:
            printf("Unrecognized: %d\n", cert->sig_type);
            break;
    }

    printf("Verify:\t\t\t");

    if (ca_cert_ctx)
    {
        x509_display_error(x509_verify(ca_cert_ctx, cert));
    }

    printf("\n");
#if 0
    print_blob("Signature", cert->signature, cert->sig_len);
    bi_print("Modulus", cert->rsa_ctx->m);
    bi_print("Pub Exp", cert->rsa_ctx->e);
#endif

    if (ca_cert_ctx)
    {
        x509_print(ca_cert_ctx, cert->next);
    }
}

void x509_display_error(int error)
{
    switch (error)
    {
        case X509_NOT_OK:
            printf("X509 not ok");
            break;

        case X509_VFY_ERROR_NO_TRUSTED_CERT:
            printf("No trusted cert is available");
            break;

        case X509_VFY_ERROR_BAD_SIGNATURE:
            printf("Bad signature");
            break;

        case X509_VFY_ERROR_NOT_YET_VALID:
            printf("Cert is not yet valid");
            break;

        case X509_VFY_ERROR_EXPIRED:
            printf("Cert has expired");
            break;

        case X509_VFY_ERROR_SELF_SIGNED:
            printf("Cert is self-signed");
            break;

        case X509_VFY_ERROR_INVALID_CHAIN:
            printf("Chain is invalid (check order of certs)");
            break;

        case X509_VFY_ERROR_UNSUPPORTED_DIGEST:
            printf("Unsupported digest");
            break;

        case X509_INVALID_PRIV_KEY:
            printf("Invalid private key");
            break;
    }
}
#endif      /* CONFIG_SSL_FULL_MODE */


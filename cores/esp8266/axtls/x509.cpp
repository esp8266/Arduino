/*
 * Copyright (c) 2007-2016, Cameron Rich
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution.
 * * Neither the name of the axTLS project nor the names of its contributors 
 *   may be used to endorse or promote products derived from this software 
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file x509.c
 * 
 * Certificate processing.
 */

#include <Arduino.h>
#include <time.h>
#include "asn1.h"
#include "sha256.h"
#include "x509.h"

static int x509_v3_subject_alt_name(const uint8_t *cert, int offset, 
        X509_CTX *x509_ctx);
static int x509_v3_basic_constraints(const uint8_t *cert, int offset, 
        X509_CTX *x509_ctx);
static int x509_v3_key_usage(const uint8_t *cert, int offset, 
        X509_CTX *x509_ctx);

/**
 * Retrieve the signature from a certificate.
 */
static const uint8_t *get_signature(const uint8_t *asn1_sig, int *len)
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

/**
 * Construct a new x509 object.
 * @return 0 if ok. < 0 if there was a problem.
 */
int x509_new(const uint8_t *cert, int *len, X509_CTX **ctx)
{
    int begin_tbs, end_tbs;
    int ret = X509_NOT_OK, offset = 0, cert_size = 0;
    int version = 0;
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

    /* optional version */
    if (cert[offset] == ASN1_EXPLICIT_TAG && 
            asn1_version(cert, &offset, &version) == X509_NOT_OK)
        goto end_cert;

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
    {
        goto end_cert;
    }

    bi_ctx = x509_ctx->rsa_ctx->bi_ctx;

    /* use the appropriate signature algorithm */
    switch (x509_ctx->sig_type)
    {
        /*
        case SIG_TYPE_MD5:
        {
            MD5_CTX md5_ctx;
            uint8_t md5_dgst[MD5_SIZE];
            MD5_Init(&md5_ctx);
            MD5_Update(&md5_ctx, &cert[begin_tbs], end_tbs-begin_tbs);
            MD5_Final(md5_dgst, &md5_ctx);
            x509_ctx->digest = bi_import(bi_ctx, md5_dgst, MD5_SIZE);
        }
            break;

        case SIG_TYPE_SHA1:
        {
            SHA1_CTX sha_ctx;
            uint8_t sha_dgst[SHA1_SIZE];
            SHA1_Init(&sha_ctx);
            SHA1_Update(&sha_ctx, &cert[begin_tbs], end_tbs-begin_tbs);
            SHA1_Final(sha_dgst, &sha_ctx);
            x509_ctx->digest = bi_import(bi_ctx, sha_dgst, SHA1_SIZE);
        }
            break;
        */
        case SIG_TYPE_SHA256:
        {
            SHA256_CTX sha256_ctx;
            uint8_t sha256_dgst[SHA256_SIZE];
            SHA256_Init(&sha256_ctx);
            SHA256_Update(&sha256_ctx, &cert[begin_tbs], end_tbs-begin_tbs);
            SHA256_Final(sha256_dgst, &sha256_ctx);
            x509_ctx->digest = bi_import(bi_ctx, sha256_dgst, SHA256_SIZE);
        }
            break;
        /*
        case SIG_TYPE_SHA384:
        {
            SHA384_CTX sha384_ctx;
            uint8_t sha384_dgst[SHA384_SIZE];
            SHA384_Init(&sha384_ctx);
            SHA384_Update(&sha384_ctx, &cert[begin_tbs], end_tbs-begin_tbs);
            SHA384_Final(sha384_dgst, &sha384_ctx);
            x509_ctx->digest = bi_import(bi_ctx, sha384_dgst, SHA384_SIZE);
        }
            break;

        case SIG_TYPE_SHA512:
        {
            SHA512_CTX sha512_ctx;
            uint8_t sha512_dgst[SHA512_SIZE];
            SHA512_Init(&sha512_ctx);
            SHA512_Update(&sha512_ctx, &cert[begin_tbs], end_tbs-begin_tbs);
            SHA512_Final(sha512_dgst, &sha512_ctx);
            x509_ctx->digest = bi_import(bi_ctx, sha512_dgst, SHA512_SIZE);
        }
            break;
        */
    }

    if (version == 2 && asn1_next_obj(cert, &offset, ASN1_V3_DATA) > 0)
    {
        x509_v3_subject_alt_name(cert, offset, x509_ctx);
        x509_v3_basic_constraints(cert, offset, x509_ctx);
        x509_v3_key_usage(cert, offset, x509_ctx);
    }

    offset = end_tbs;   /* skip the rest of v3 data */
    if (asn1_skip_obj(cert, &offset, ASN1_SEQUENCE) || asn1_signature(cert, &offset, x509_ctx))
      goto end_cert;

    ret = X509_OK;
    
end_cert:
    if (len)
    {
        *len = cert_size;
    }

    if (ret)
    {
        x509_free(x509_ctx);
        *ctx = NULL;
    }

    return ret;
}

static int x509_v3_subject_alt_name(const uint8_t *cert, int offset, 
        X509_CTX *x509_ctx)
{
    if ((offset = asn1_is_subject_alt_name(cert, offset)) > 0)
    {
        x509_ctx->subject_alt_name_present = true;
        x509_ctx->subject_alt_name_is_critical = 
                        asn1_is_critical_ext(cert, &offset);

        if (asn1_next_obj(cert, &offset, ASN1_OCTET_STRING) > 0)
        {
            int altlen;

            if ((altlen = asn1_next_obj(cert, &offset, ASN1_SEQUENCE)) > 0)
            {
                int endalt = offset + altlen;
                int totalnames = 0;

                while (offset < endalt)
                {
                    int type = cert[offset++];
                    int dnslen = get_asn1_length(cert, &offset);

                    if (type == ASN1_CONTEXT_DNSNAME)
                    {
                        x509_ctx->subject_alt_dnsnames = (char**)
                                realloc(x509_ctx->subject_alt_dnsnames, 
                                   (totalnames + 2) * sizeof(char*));
                        x509_ctx->subject_alt_dnsnames[totalnames] = 
                                (char*)malloc(dnslen + 1);
                        x509_ctx->subject_alt_dnsnames[totalnames+1] = NULL;
                        memcpy(x509_ctx->subject_alt_dnsnames[totalnames], 
                                cert + offset, dnslen);
                        x509_ctx->subject_alt_dnsnames[totalnames][dnslen] = 0;
                        totalnames++;
                    }

                    offset += dnslen;
                }
            }
        }
    }

    return X509_OK;
}

/**
 * Basic constraints - see https://tools.ietf.org/html/rfc5280#page-39
 */
static int x509_v3_basic_constraints(const uint8_t *cert, int offset, 
        X509_CTX *x509_ctx)
{
    int ret = X509_OK;

    if ((offset = asn1_is_basic_constraints(cert, offset)) == 0)
        goto end_contraints;

    x509_ctx->basic_constraint_present = true;
    x509_ctx->basic_constraint_is_critical = 
                    asn1_is_critical_ext(cert, &offset);

    if (asn1_next_obj(cert, &offset, ASN1_OCTET_STRING) < 0 ||
    		asn1_next_obj(cert, &offset, ASN1_SEQUENCE) < 0 ||
    		asn1_get_bool(cert, &offset, &x509_ctx->basic_constraint_cA) < 0 ||
    		asn1_get_int(cert, &offset,
    		                &x509_ctx->basic_constraint_pathLenConstraint) < 0)
    {
        ret = X509_NOT_OK;       
    }

end_contraints:
    return ret;
}

/*
 * Key usage - see https://tools.ietf.org/html/rfc5280#section-4.2.1.3
 */
static int x509_v3_key_usage(const uint8_t *cert, int offset, 
        X509_CTX *x509_ctx)
{
    int ret = X509_OK;

    if ((offset = asn1_is_key_usage(cert, offset)) == 0)
        goto end_key_usage;

    x509_ctx->key_usage_present = true;
    x509_ctx->key_usage_is_critical = asn1_is_critical_ext(cert, &offset);

    if (asn1_next_obj(cert, &offset, ASN1_OCTET_STRING) < 0 ||
    		asn1_get_bit_string_as_int(cert, &offset, &x509_ctx->key_usage))
    {
        ret = X509_NOT_OK;       
    }

end_key_usage:
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

    if (x509_ctx->digest)
    {
        bi_free(x509_ctx->rsa_ctx->bi_ctx, x509_ctx->digest);
    }

    if (x509_ctx->subject_alt_dnsnames)
    {
        for (i = 0; x509_ctx->subject_alt_dnsnames[i]; ++i)
            free(x509_ctx->subject_alt_dnsnames[i]);

        free(x509_ctx->subject_alt_dnsnames);
    }

    RSA_free(x509_ctx->rsa_ctx);
    next = x509_ctx->next;
    free(x509_ctx);
    x509_free(next);        /* clear the chain */
}

/**
 * Take a signature and decrypt it.
 */
static bigint *sig_verify(BI_CTX *ctx, const uint8_t *sig, int sig_len,
        bigint *modulus, bigint *pub_exp)
{
    int i, size;
    bigint *decrypted_bi, *dat_bi;
    bigint *bir = NULL;
    uint8_t *block = (uint8_t *)alloca(sig_len);

    /* decrypt */
    dat_bi = bi_import(ctx, sig, sig_len);
    ctx->mod_offset = BIGINT_M_OFFSET;

    /* convert to a normal block */
    decrypted_bi = bi_mod_power2(ctx, dat_bi, modulus, pub_exp);

    bi_export(ctx, decrypted_bi, block, sig_len);
    ctx->mod_offset = BIGINT_M_OFFSET;

    i = 10; /* start at the first possible non-padded byte */
    while (block[i++] && i < sig_len);
    size = sig_len - i;

    /* get only the bit we want */
    if (size > 0)
    {
        int len;
        const uint8_t *sig_ptr = get_signature(&block[i], &len);

        if (sig_ptr)
        {
            bir = bi_import(ctx, sig_ptr, len);
        }
    }

    /* save a few bytes of memory */
    bi_clear_cache(ctx);
    return bir;
}

/**
 * Do some basic checks on the certificate chain.
 *
 * Certificate verification consists of a number of checks:
 * - The date of the certificate is after the start date.
 * - The date of the certificate is before the finish date.
 * - A root certificate exists in the certificate store.
 * - That the certificate(s) are not self-signed.
 * - The certificate chain is valid.
 * - The signature of the certificate is valid.
 * - Basic constraints 
 */
int x509_verify(const CA_CERT_CTX *ca_cert_ctx, const X509_CTX *cert, int *pathLenConstraint) 
{
    int ret = X509_OK, i = 0;
    bigint *cert_sig;
    X509_CTX *next_cert = NULL;
    BI_CTX *ctx = NULL;
    bigint *mod = NULL, *expn = NULL;
    int match_ca_cert = 0;
    //struct timeval tv;
    uint8_t is_self_signed = 0;

    if (cert == NULL)
    {
        ret = X509_VFY_ERROR_NO_TRUSTED_CERT;       
        goto end_verify;
    }

    /* a self-signed certificate that is not in the CA store - use this 
       to check the signature */
    if (asn1_compare_dn(cert->ca_cert_dn, cert->cert_dn) == 0)
    {
        is_self_signed = 1;
        ctx = cert->rsa_ctx->bi_ctx;
        mod = cert->rsa_ctx->m;
        expn = cert->rsa_ctx->e;
    }

    /*
    gettimeofday(&tv, NULL);

    / * check the not before date * /
    if (tv.tv_sec < cert->not_before)
    {
        ret = X509_VFY_ERROR_NOT_YET_VALID;
        goto end_verify;
    }

    / * check the not after date * /
    if (tv.tv_sec > cert->not_after)
    {
        ret = X509_VFY_ERROR_EXPIRED;
        goto end_verify;
    }
    */

    if (cert->basic_constraint_present)
    {
    	/* If the cA boolean is not asserted,
    	   then the keyCertSign bit in the key usage extension MUST NOT be
    	   asserted. */
    	if (!cert->basic_constraint_cA &&
                IS_SET_KEY_USAGE_FLAG(cert, KEY_USAGE_KEY_CERT_SIGN))
		{
			ret = X509_VFY_ERROR_BASIC_CONSTRAINT;
			goto end_verify;
		}

        /* The pathLenConstraint field is meaningful only if the cA boolean is
           asserted and the key usage extension, if present, asserts the
           keyCertSign bit.  In this case, it gives the maximum number of 
           non-self-issued intermediate certificates that may follow this 
           certificate in a valid certification path. */
		if (cert->basic_constraint_cA &&
            (!cert->key_usage_present || 
                IS_SET_KEY_USAGE_FLAG(cert, KEY_USAGE_KEY_CERT_SIGN)) &&
            (cert->basic_constraint_pathLenConstraint+1) < *pathLenConstraint)
		{
			ret = X509_VFY_ERROR_BASIC_CONSTRAINT;
			goto end_verify;
		}
    }

    next_cert = cert->next;

    /* last cert in the chain - look for a trusted cert */
    if (next_cert == NULL)
    {
       if (ca_cert_ctx != NULL) 
       {
            /* go thru the CA store */
            while (i < CONFIG_X509_MAX_CA_CERTS && ca_cert_ctx->cert[i])
            {
                /* the extension is present but the cA boolean is not 
                   asserted, then the certified public key MUST NOT be used 
                   to verify certificate signatures. */
                if (cert->basic_constraint_present && 
                        !ca_cert_ctx->cert[i]->basic_constraint_cA)
                    continue;
                        
                if (asn1_compare_dn(cert->ca_cert_dn,
                                            ca_cert_ctx->cert[i]->cert_dn) == 0)
                {
                    /* use this CA certificate for signature verification */
                    match_ca_cert = true;
                    ctx = ca_cert_ctx->cert[i]->rsa_ctx->bi_ctx;
                    mod = ca_cert_ctx->cert[i]->rsa_ctx->m;
                    expn = ca_cert_ctx->cert[i]->rsa_ctx->e;


                    break;
                }

                i++;
            }
        }

        /* couldn't find a trusted cert (& let self-signed errors 
           be returned) */
        if (!match_ca_cert && !is_self_signed)
        {
            ret = X509_VFY_ERROR_NO_TRUSTED_CERT;       
            goto end_verify;
        }
    }
    else if (asn1_compare_dn(cert->ca_cert_dn, next_cert->cert_dn) != 0)
    {
        /* check the chain */
        ret = X509_VFY_ERROR_INVALID_CHAIN;
        goto end_verify;
    }
    else /* use the next certificate in the chain for signature verify */
    {
        ctx = next_cert->rsa_ctx->bi_ctx;
        mod = next_cert->rsa_ctx->m;
        expn = next_cert->rsa_ctx->e;
    }

    /* cert is self signed */
    if (!match_ca_cert && is_self_signed)
    {
        ret = X509_VFY_ERROR_SELF_SIGNED;
        goto end_verify;
    }

    /* check the signature */
    cert_sig = sig_verify(ctx, cert->signature, cert->sig_len, 
                        bi_clone(ctx, mod), bi_clone(ctx, expn));

    if (cert_sig && cert->digest)
    {
        if (bi_compare(cert_sig, cert->digest) != 0)
            ret = X509_VFY_ERROR_BAD_SIGNATURE;


        bi_free(ctx, cert_sig);
    }
    else
    {
        ret = X509_VFY_ERROR_BAD_SIGNATURE;
    }

    if (ret)
        goto end_verify;

    /* go down the certificate chain using recursion. */
    if (next_cert != NULL)
    {
    	(*pathLenConstraint)++; /* don't include last certificate */
        ret = x509_verify(ca_cert_ctx, next_cert, pathLenConstraint);
    }

end_verify:
    return ret;
}

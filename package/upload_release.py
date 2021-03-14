#!/usr/bin/env python3

from github import Github
import argparse

parser = argparse.ArgumentParser(description='Upload a set of files to a new draft release')
parser.add_argument('--user', help="Github username", type=str, required=True)
parser.add_argument('--token', help="Github Personal Access Token (PAT)", type=str, required=True)
parser.add_argument('--repo', help="Repository", type=str, required=True)
parser.add_argument('--tag', help="Release tag", type=str, required=True)
parser.add_argument('--name', help="Release name", type=str, required=True)
parser.add_argument('--msg', help="Release message", type=str, required=True)
parser.add_argument('files', nargs=argparse.REMAINDER)
args = parser.parse_args()

if len(args.files) == 0:
    print("ERROR:  No files specified")
    quit()

gh = Github(login_or_token=args.token)
repo = gh.get_repo(str(args.repo))
release = repo.create_git_release(args.tag, args.name, args.msg, draft=True)
for fn in args.files:
    print("Uploading file: " + fn)
    release.upload_asset(fn)

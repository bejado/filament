from github import Github
import os, sys

def print_usage():
    print('Upload artifacts to a Filament GitHub release.')
    print()
    print('Usage:')
    print('  upload-artifacts.py <tag> <artifact>...')
    print()
    print('Notes:')
    print('  The GitHub release must already be created prior to running this script. This is typically done')
    print('  through the GitHub web UI.')
    print()
    print('  <tag> is the Git tag for the desired release to attach artifacts to, for example, "v1.4.2".')
    print()
    print('  <artifact> is a path to the asset file to upload. The file name will be used as the name of the')
    print('  asset.')
    print()
    print('  The GITHUB_API_KEY environment variable must be set to a valid authentication token for a')
    print('  collaborator account of the Filament repository.')

# The first argument is the path to this script.
if len(sys.argv) < 3:
    print_usage()
    sys.exit(1)

tag = sys.argv[1]
artifacts = sys.argv[2:]

authentication_token = os.environ.get('GITHUB_API_KEY')
if not authentication_token:
    sys.stderr.write('Error: the GITHUB_API_KEY is not set.\n')
    sys.exit(1)

g = Github(authentication_token)

# TODO: be sure to change to google/filament after testing
FILAMENT_REPO = "bejado/filament"
filament = g.get_repo(FILAMENT_REPO)

def find_release_from_tag(repo, tag):
    """ Find a release in the repo for the given Git tag string. """
    releases = repo.get_releases()
    for r in releases:
        if r.tag_name == tag:
            return r
    return None

release = find_release_from_tag(filament, tag)
if not release:
    sys.stderr.write(f"Error: Could not find release with tag '{tag}'.\n")
    sys.exit(1)

print(f"Found release with tag '{tag}'.")

for artifact_path in artifacts:
    sys.stdout.write(f'Uploding artifact: {artifact_path}... ')
    asset_name = os.path.basename(artifact_path)
    asset = release.upload_asset(artifact_path, name=asset_name)
    if asset:
        sys.stdout.write('Success!\n')

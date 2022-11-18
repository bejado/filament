const uploadReleaseAssets = async ({github, context}, assetPathsToUpload, releaseTag) => {
    const fs = require('fs/promises');
    const path = require('path');

    const filesToUpload = assetPathsToUpload.map(assetPath => ({
        buffer: fs.readFile(assetPath),
        assetName: path.basename(assetPath)
    }));

    const findReleaseMatchingTag = async tag => {
        const { data: releases } = await github.rest.repos.listReleases(context.repo);
        const release = releases.find(release => release.tag_name === releaseTag);
        if (!release) {
            throw new Error(`Could not locate release with tag '${releaseTag}'`);
        }
        return release;
    };

    const release = await findReleaseMatchingTag(releaseTag);

    console.log(`Found release named ${release.name} matching tag ${release.tag_name}.`);

    const uploadPromises = [];
    for (const file of filesToUpload) {
        console.log(`Uploading asset ${file.assetName}.`);
        const p = github.rest.repos.uploadReleaseAsset({
            ...context.repo,
            release_id: release.id,
            name: file.assetName,
            data: await file.buffer
        });
        uploadPromises.push(p);
    }

    await Promise.all(uploadPromises);
    console.log("Done!");
}

module.exports = uploadReleaseAssets;

////////////////////////////////////////////////////////////////////////////////////////////////////

// const { Octokit } = require("@octokit/rest");
// const glob = require("@actions/glob");
// (async () => {
//     const github = new Octokit({
//         auth: ""
//     });
//     const context = {
//         repo: {
//             owner: "bejado",
//             repo: "filament",
//         },
//     };
//     const globber = await glob.create('*.txt');
//     await uploadReleaseAssets({ github, context }, await globber.glob(), 'vtest-28');
// })();
#!/bin/bash

folderPath=$1
mean=$2
outFolderPath=$3
outMeanFile=$4

for file in $(ls ${folderPath}); do
    ./${mean} < "${folderPath}/${file}" > "./${outFolderPath}/mean_${file}"
    echo $file >> "./$outMeanFile"
    cat "./${outFolderPath}/mean_${file}" >> "./$outMeanFile"
done

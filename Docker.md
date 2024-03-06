 # Docker image build

## Prequesits
Docker install 
Example (Docker desktop for windows: https://www.docker.com/products/docker-desktop/) (needs hypervisor ensable in bios and WSL)



## Build

get binaries in ./Release/DarwinServer.exe

docker build --pull --rm -f "Dockerfile" -t darwin:latest "." 


## Publish:

* ref https://docs.github.com/en/actions/publishing-packages/publishing-docker-images
* ref https://docs.docker.com/get-started/04_sharing_app/

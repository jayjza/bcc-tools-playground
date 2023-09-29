#/bin/bash
sudo docker run -ti --rm\
    --cap-add=NET_ADMIN \
    --network=host \
    --privileged \
    -v /etc/localtime:/etc/localtime:ro \
    -v /Users/jay/Work/hackdays/bcc-tools-docker/custom-bcc-tools:/bcc-playground/custom-bcc-tools bcc-playground
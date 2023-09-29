#/bin/bash
sudo docker run -ti --rm\
    --cap-add=NET_ADMIN \
    --network=host \
    --privileged \
    --name bcc \
    --pid=host \
    -v /sys/kernel/debug:/sys/kernel/debug:rw \
    -v /etc/localtime:/etc/localtime:ro \
    -v ./custom-bcc-tools:/bcc-playground/custom-bcc-tools \
    bcc-playground
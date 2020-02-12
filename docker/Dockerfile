FROM ubuntu:latest
add etc /home/work/
add lib /lib
add sources.list /etc/apt/
add mailServer /bin
RUN apt-get update && apt-get -y install default-libmysqlclient-dev && apt-get -y install libgoogle-glog0v5
CMD ["/bin/bash"]

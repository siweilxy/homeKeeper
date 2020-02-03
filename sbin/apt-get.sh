sudo apt-get -o Acquire::http::proxy="http://127.0.0.1:45441"  install nvidia-driver-415

sudo apt-get -o Acquire::http::proxy="http://127.0.0.1:45441"  update

sudo apt-get -o Acquire::http::proxy="http://127.0.0.1:45441" -y  install   cuda


git config --global http.proxy "http://127.0.0.1:45441"
git config --global https.proxy "https://127.0.0.1:45441"
git config --global http.sslVerify false

git config --global --unset http.proxy
git config --global --unset https.proxy

FROM golang:alpine
RUN  apk add --update --no-cache make g++
WORKDIR /go/src/github.com/rikugun/softkey
COPY . ./
RUN cd lib/softkey && make 
ENV GOPROXY=https://goproxy.cn,https://goproxy.io,direct \
    GO111MODULE=on \
    CGO_ENABLED=1
RUN go env -w GOPROXY=https://goproxy.cn,https://goproxy.io,direct
RUN go env && go build -v -a . 
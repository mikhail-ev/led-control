FROM node:13

WORKDIR /usr/src/app

COPY package*.json ./

RUN npm install

COPY server.js server.js

COPY public public

CMD [ "node", "server.js" ]
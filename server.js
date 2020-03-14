const express = require('express');
const app = express();
const port = process.env.PORT || 3000;

let requestCounter = 0;

app.use((...args) => {
    console.log(`[${requestCounter++}][${new Date().toLocaleTimeString()}]${args[0].url}`);
    express.static('public')(...args);
})

app.get('*', (req, res) => res.redirect('/'))

app.listen(port, () => console.log(`Example app listening on port ${port}!`))
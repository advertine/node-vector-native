Vector bindings for node
========================


Installation:
-------------

[![Build Status][BS img]][Build Status]

```
npm install advertine/vector-native
```

Tested on Linux (x64), OS X and MS Windows (x64 and x86).

Usage:
------

```js

var Vector = require('vector-native')
var v = new Vector()
v[1] = 0.5;
var b = new Vector({1: 0.5, 2: 0.25});
console.log(v.toObject());
v.add(b);
console.log(v.toObject());

```


Bugs, limitations, caveats
--------------------------

Tested with nodejs: v0.10, v0.11, v0.12 and iojs.

[Build Status]: https://travis-ci.org/advertine/node-vector-native
[BS img]: https://travis-ci.org/advertine/node-vector-native.svg

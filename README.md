Vector bindings for node
========================


Installation:
-------------

```
npm install vector-native
```

Tested on Linux (x64), OS X and MS Windows (x64 and x86).


Usage:
------

```js
var Vector = require('./index')
var v = new Vector()
v[1] = 0.5;
var b = new Vector({1: 0.5, 2: 0.25}).getBuffer();
v.toObject()
v.add(b);
v.toObject()

var vec1 = new Vector()
var vec2 = new Vector(buffer)
var vec3 = new Vector({dimX => N})

vec1.add(vec2, vec3) -> vec4
vec1.add([vec2, vec3]) -> vec4

vec1.toObject() -> {dimX => N}

vec1[1234]
vec1[1234] = 0.5
vec1.buffer

vec1.mul()

```



Bugs, limitations, caveats
--------------------------

Tested with nodejs: v0.8, v0.10, v0.11, v0.12 and iojs.

[Build Status]: https://travis-ci.org/royaltm/node-vector-native
[BS img]: https://travis-ci.org/royaltm/node-vector-native.svg

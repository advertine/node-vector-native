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
v.equals(b);
v.multiply(b);
v.multiply(10);
v.normalize();
v.scalar(b);
v.average;
v.variance;
v.length;
v.valueOf();
console.log(v.toObject());
console.log(v.getBuffer());
new Vector(v.getBuffer())

```

Use in your own native extension
--------------------------------

in bindings.gyp:

```python
      ...
      'variables': {
        'VECTOR_NATIVE_DIR%': "<!(node -e \"require('vector-native/include_dirs')\")"
      },
      'include_dirs': [
        "<!(node -e \"require('nan')\")",
        '<(VECTOR_NATIVE_DIR)',
        ...
      ]
      'sources': [
        '<(VECTOR_NATIVE_DIR)/vector.cc',
        '<(VECTOR_NATIVE_DIR)/nodevector.cc',
        ...
      ]
```

Bugs, limitations, caveats
--------------------------

Tested with nodejs: v0.10, v0.11, v0.12, iojs-3 and v4.

[Build Status]: https://travis-ci.org/advertine/node-vector-native
[BS img]: https://travis-ci.org/advertine/node-vector-native.svg

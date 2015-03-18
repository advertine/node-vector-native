var test = require("tap").test
  , Vector = require('..')
  , endianness = require('os').endianness()
  , readUInt32 = 'readUInt32' + endianness
  , readDouble = 'readDouble' + endianness
;

test("should have Vector function", function(t) {
  t.type(Vector, 'function');
  t.end();
});

test("Vector should have dimension getters and setters", function(t) {
  var vec = new Vector();
  t.strictEqual(vec.length, 0);
  t.strictEqual(vec[0], 0);
  t.strictEqual(vec[(Math.random()*0x7FFFFFFF)|0], 0);
  t.strictEqual(vec.length, 0);
  t.strictEqual(vec[0] = 1, 1);
  t.strictEqual(vec[0], 1);
  t.strictEqual(vec.length, 1);
  t.strictEqual(vec[10] = 0.5, 0.5);
  t.strictEqual(vec[10], 0.5);
  t.strictEqual(vec.length, 2);
  t.strictEqual(vec[0] = 0, 0);
  t.strictEqual(vec.length, 1);
  t.strictEqual(vec[0] = 0, 0);
  t.strictEqual(vec[10] = 0, 0);
  t.strictEqual(vec.length, 0);
  t.end();
});

test("Vector should initialize from Vector, Object or Buffer", function(t) {
  var vec = new Vector({7: 0.25, 5: 0.75});
  t.strictEqual(vec.length, 2);
  t.strictEqual(vec[7], 0.25);
  t.strictEqual(vec[5], 0.75);

  var vec2 = new Vector(vec);
  t.notStrictEqual(vec, vec2);
  t.strictEqual(vec2.length, 2);
  t.strictEqual(vec2[7], 0.25);
  t.strictEqual(vec2[5], 0.75);

  var buf = vec2.getBuffer();

  t.type(buf, Buffer);
  t.equal(buf.length, Vector.BYTEARRAY_ELEMENT_SIZE * 2);

  var vec3 = new Vector(buf);
  t.notStrictEqual(vec2, vec3);
  t.strictEqual(vec3.length, 2);
  t.strictEqual(vec3[7], 0.25);
  t.strictEqual(vec3[5], 0.75);

  t.end();
});

test("Vector should convert to Object", function(t) {
  var vec = new Vector();
  t.deepEqual(vec.toObject(), {});

  vec[7] = 0.25;
  vec[5] = 0.75;
  vec[3] = 0.15;
  t.deepEqual(vec.toObject(), {3: 0.15, 5: 0.75, 7: 0.25});

  var vec2 = new Vector({7: 0.25, 5: 0.75, 3: 0.15});
  t.deepEqual(vec2.toObject(), {3: 0.15, 5: 0.75, 7: 0.25});

  t.end();
});

test("Vector should convert to byte-array Buffer", function(t) {
  var vec = new Vector();
  t.deepEqual(vec.getBuffer(), new Buffer(0));

  vec[7] = 0.25;
  vec[5] = 0.75;
  vec[3] = 0.15;

  var buf = vec.getBuffer();

  t.strictEqual(buf.length, Vector.BYTEARRAY_ELEMENT_SIZE * vec.length);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_DIM_OFFSET), 3);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_VALUE_OFFSET), 0.15);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_ELEMENT_SIZE * 1 + Vector.BYTEARRAY_DIM_OFFSET), 5);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_ELEMENT_SIZE * 1 + Vector.BYTEARRAY_VALUE_OFFSET), 0.75);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_ELEMENT_SIZE * 2 + Vector.BYTEARRAY_DIM_OFFSET), 7);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_ELEMENT_SIZE * 2 + Vector.BYTEARRAY_VALUE_OFFSET), 0.25);

  var buf = vec.getBuffer(null, true);

  t.strictEqual(buf.length, Vector.BYTEARRAY_ELEMENT_SIZE * vec.length);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_DIM_OFFSET), 5);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_VALUE_OFFSET), 0.75);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_ELEMENT_SIZE * 1 + Vector.BYTEARRAY_DIM_OFFSET), 7);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_ELEMENT_SIZE * 1 + Vector.BYTEARRAY_VALUE_OFFSET), 0.25);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_ELEMENT_SIZE * 2 + Vector.BYTEARRAY_DIM_OFFSET), 3);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_ELEMENT_SIZE * 2 + Vector.BYTEARRAY_VALUE_OFFSET), 0.15);

  var buf = vec.getBuffer(null, 2);

  t.strictEqual(buf.length, Vector.BYTEARRAY_ELEMENT_SIZE * 2);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_DIM_OFFSET), 5);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_VALUE_OFFSET), 0.75);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_ELEMENT_SIZE * 1 + Vector.BYTEARRAY_DIM_OFFSET), 7);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_ELEMENT_SIZE * 1 + Vector.BYTEARRAY_VALUE_OFFSET), 0.25);

  var buf = vec.getBuffer(null, 1);

  t.strictEqual(buf.length, Vector.BYTEARRAY_ELEMENT_SIZE);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_DIM_OFFSET), 5);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_VALUE_OFFSET), 0.75);

  t.end();
});

test("Vector should have valueOf, average, sigma, and variance", function(t) {
  var vec = new Vector({7: 0.25, 5: 0.75, 3: 0.15});

  t.equal(+vec, Math.sqrt(0.25*0.25 + 0.75*0.75 + 0.15*0.15));
  t.equal(vec.average, (0.25 + 0.75 + 0.15) / 3);
  t.equal(vec.variance, (0.25*0.25 + 0.75*0.75 + 0.15*0.15)/3 - vec.average*vec.average);
  t.equal(vec.sigma, Math.sqrt(vec.variance));

  t.end();
});


test("Vector should convert to byte-array Buffer with sigma cutoff", function(t) {
  var vec = new Vector({7: 0.75, 5: 0.25, 3: 0.15});

  var buf = vec.getBuffer(0.5);

  t.strictEqual(buf.length, Vector.BYTEARRAY_ELEMENT_SIZE);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_DIM_OFFSET), 7);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_VALUE_OFFSET), 0.75);

  var buf = vec.getBuffer(0.75);

  t.strictEqual(buf.length, Vector.BYTEARRAY_ELEMENT_SIZE * 2);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_DIM_OFFSET), 5);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_VALUE_OFFSET), 0.25);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_ELEMENT_SIZE * 1 + Vector.BYTEARRAY_DIM_OFFSET), 7);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_ELEMENT_SIZE * 1 + Vector.BYTEARRAY_VALUE_OFFSET), 0.75);

  var buf = vec.getBuffer(0.75, true);

  t.strictEqual(buf.length, Vector.BYTEARRAY_ELEMENT_SIZE * 2);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_DIM_OFFSET), 7);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_VALUE_OFFSET), 0.75);
  t.strictEqual(buf[readUInt32](Vector.BYTEARRAY_ELEMENT_SIZE * 1 + Vector.BYTEARRAY_DIM_OFFSET), 5);
  t.strictEqual(buf[readDouble](Vector.BYTEARRAY_ELEMENT_SIZE * 1 + Vector.BYTEARRAY_VALUE_OFFSET), 0.25);

  t.end();
});

test("Vector should normalize", function(t) {
  var vec = new Vector({1: 0.5});

  t.strictEqual(vec.valueOf(), 0.5);
  t.strictEqual(vec.normalize(), vec);
  t.strictEqual(vec.valueOf(), 1);
  t.strictEqual(vec[1], 1);
  t.strictEqual(vec.normalize(0.25), vec);
  t.strictEqual(vec.valueOf(), 4);
  t.strictEqual(vec[1], 4);
  t.strictEqual(vec.normalize(2), vec);
  t.strictEqual(vec.valueOf(), 2);
  t.strictEqual(vec[1], 2);

  t.end();
});

test("Vector should add other vectors", function(t) {
  var vec1 = new Vector();
  var vec2 = new Vector({1: 0.5});

  t.strictEqual(vec1.valueOf(), 0);
  t.strictEqual(vec1.length, 0);
  t.strictEqual(vec1.add(vec2), vec1);
  t.notStrictEqual(vec1, vec2);
  t.strictEqual(vec1.length, 1);
  t.strictEqual(vec1.valueOf(), 0.5);
  t.strictEqual(vec1[1], 0.5);

  vec2[1] = 1;
  t.strictEqual(vec1.add(vec2), vec1);
  t.notStrictEqual(vec1, vec2);
  t.strictEqual(vec1.length, 1);
  t.strictEqual(vec1.valueOf(), 1.5);
  t.strictEqual(vec1[1], 1.5);

  vec2[1] = 0;
  vec2[2] = 0.5;
  var buf = vec2.getBuffer();
  t.strictEqual(vec1.add(buf), vec1);
  t.notStrictEqual(vec1, vec2);
  t.strictEqual(vec1.length, 2);
  t.strictEqual(vec1.valueOf(), Math.sqrt(0.25 + 2.25));
  t.strictEqual(vec1[1], 1.5);
  t.strictEqual(vec1[2], 0.5);

  t.end();
});

test("Vector should be multiplied by other vectors", function(t) {
  var vec1 = new Vector({1: 0.5, 2: 1});
  var vec2 = new Vector({1: 2});

  t.strictEqual(vec1.length, 2);
  t.strictEqual(vec1.multiply(vec2), vec1);
  t.notStrictEqual(vec1, vec2);
  t.strictEqual(vec1.length, 1);
  t.strictEqual(vec1.valueOf(), 1);
  t.strictEqual(vec1[1], 1);
  t.strictEqual(vec1[2], 0);

  t.end();
});

test("Vector should be multiplied by scalar", function(t) {
  var vec1 = new Vector({1: 0.5, 2: 1});

  t.strictEqual(vec1.length, 2);
  t.strictEqual(vec1.multiply(2), vec1);
  t.strictEqual(vec1.length, 2);
  t.strictEqual(vec1.valueOf(), Math.sqrt(5));
  t.strictEqual(vec1[1], 1);
  t.strictEqual(vec1[2], 2);

  t.end();
});

test("Vector should have scalar product of vectors", function(t) {
  var vec1 = new Vector({1: 0.5, 2: 1});
  var vec2 = new Vector({1: 2});

  t.strictEqual(vec1.scalar(vec2), 1);

  t.end();
});

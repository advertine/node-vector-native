#!/usr/bin/env node
var ben            = require('ben')
,   vectpr         = require('..')
,   assert         = require('assert')
,   iters          = 100000
,   crypto         = require('crypto')
,   createHash     = crypto.createHash
,   stringEncoding = process.argv[2]

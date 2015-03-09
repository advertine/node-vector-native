{
  'targets': [
    {
      'target_name': 'nodevector',
      'sources': [
        'src/vector.cc',
        'src/nodevector.cc'
      ],
      'include_dirs': [
        "<!(node -e \"require('nan')\")",
        'src'
      ],
      'defines': [
      ],
      'conditions': [
        ['target_arch!="x64"', {
          'defines': [
          ]
        }],
        ['OS=="win"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': 1,
              'AdditionalOptions': [
                '/EHsc' # ExceptionHandling=1 is not enough
              ]
            }
          }
        }]
      ]
    }
  ]
}

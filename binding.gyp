{
  'targets': [
    {
      'target_name': 'nodevector',
      'sources': [
        'src/vector.cc',
        'src/nodevector.cc',
        'src/nodemodule.cc'
      ],
      'include_dirs': [
        "<!(node -e \"require('nan')\")",
        'src'
      ],
      'conditions': [
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

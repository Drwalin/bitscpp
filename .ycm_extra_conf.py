def Settings( **kwargs ):
  return {
    'flags': ['-x', 'c++', '-Wall', '-pedantic', '-Wl,--verbose',
    '-Iinclude', '-std=c++17', '-I/usr/include'],
  }

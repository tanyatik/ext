cpp_flags = ['-Wall',
             '-Wextra',
             '-pedantic',
             '-std=c++11',
             '-stdlib=libc++',
             '-pthread',
             '-g'
            ]

env = Environment(CXX = 'g++',
                  CPPFLAGS = cpp_flags,
                  CPPPATH = 'tclap-1.2.1/include'
                 )

env.Program(target = 'veb', source = ['veb.cpp'])


cpp_flags = ['-Wall',
             '-Wextra',
             '-pedantic',
             '-std=c++11',
             '-stdlib=libc++',
             '-pthread'
            ]

env = Environment(CXX = 'g++',
                  CPPFLAGS = cpp_flags,
                  CPPPATH = 'tclap-1.2.1/include'
                 )

env.Program(target = 'ext_sort', source = ['ext_sort.cpp'])

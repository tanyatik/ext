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
                 )

env.Program(target = 'sko', source = ['streaming_k_order.cpp'])


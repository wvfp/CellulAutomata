#递归将本文件夹下所有mainApplication.cpp放到MAIN中
file(GLOB_RECURSE MAIN ./  mainApplication.cpp)

#将MAIN中所有cpp编译为mainApplication这个lib库
add_library(mainApplication ${MAIN})


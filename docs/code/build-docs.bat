mkdir build
doxygen 

python -m sphinx.cmd.build -b html ./source/ ./build/

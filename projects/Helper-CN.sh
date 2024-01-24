#!/bin/bash

# 检查是否提供了目录路径作为参数
if [ $# -eq 0 ]; then
    echo "请提供目录路径作为参数。"
    exit 1
fi

# 指定目录路径为第一个参数
YOUR_DIRECTORY="$1"

# 使用find命令查找指定目录下的所有文件，然后使用while循环逐一处理
find "$YOUR_DIRECTORY" -type f -print | while IFS= read -r file; do
    # 使用sed命令替换"gcr.io"为"gcr.nju.edu.cn"
    sed -i 's/gcr\.io/gcr.m.daocloud.io/g' "$file"
    echo "替换文件 $file 中的 gcr.io 为 gcr.m.daocloud.io"
done

echo "替换完成。"

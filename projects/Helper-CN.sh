#!/bin/bash

# ����Ƿ��ṩ��Ŀ¼·����Ϊ����
if [ $# -eq 0 ]; then
    echo "���ṩĿ¼·����Ϊ������"
    exit 1
fi

# ָ��Ŀ¼·��Ϊ��һ������
YOUR_DIRECTORY="$1"

# ʹ��find�������ָ��Ŀ¼�µ������ļ���Ȼ��ʹ��whileѭ����һ����
find "$YOUR_DIRECTORY" -type f -print | while IFS= read -r file; do
    # ʹ��sed�����滻"gcr.io"Ϊ"gcr.nju.edu.cn"
    sed -i 's/gcr\.io/gcr.m.daocloud.io/g' "$file"
    echo "�滻�ļ� $file �е� gcr.io Ϊ gcr.m.daocloud.io"
done

echo "�滻��ɡ�"

if [ $# -eq 0 ]; then
    echo "Usage: $0 <test_folder_name>"
    exit 1
fi

if [ ! -d "./tests/$1" ]; then
    echo "Folder $1 does not exist in ./tests"
    exit 1
fi

for file in $(ls ./tests/$1/*.txt | sort -V); do
    echo $file
    ./parser_tester $file
    echo ""
done

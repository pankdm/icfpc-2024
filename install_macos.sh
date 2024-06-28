function configure_poetry() {
    echo '-> Updating pip'
    pip install --upgrade pip
    echo '-> Pulling deps'
    pip install poetry
}

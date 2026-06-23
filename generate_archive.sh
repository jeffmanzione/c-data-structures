PROJECT="jeffmanzione_c_data_structures"
TAG="v1.0.x"
VERSION="1.0.9"

git archive \
    --format=zip \
    --prefix "$PROJECT-$VERSION/" \
    --output "./$PROJECT-$VERSION.zip" \
    "$TAG"

git archive \
    --format="tar.gz" \
    --prefix "$PROJECT-$VERSION/" \
    --output "./$PROJECT-$VERSION.tar.gz" \
    "$TAG"
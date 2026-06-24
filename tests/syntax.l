void main(int64 p1, int64 p2) {
    int64 a
    a <- test()
    print(a)

    int64 b
    b <- a + 10
}

int64 test() {
    return 5
}
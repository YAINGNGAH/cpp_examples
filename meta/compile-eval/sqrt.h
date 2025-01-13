template <bool B, bool check, int L, int M, int R, int N, bool = ((L + M) / 2 <= 46340) || !check,
          bool = ((L + R) / 2 <= 46340) || check>
struct Search {
    static const int value = R;
};

template <int L, int M, int R, int N>
struct Search<false, true, L, M, R, N, true, true> {
    static const int value =
        Search<(R - L <= 1), ((L + M) / 2) * ((L + M) / 2) >= N, L, (L + M) / 2, M, N>::value;
};
template <int L, int M, int R, int N>
struct Search<false, true, L, M, R, N, false, true> {
    static const int value = Search<(R - L <= 1), true, L, (L + M) / 2, M, N>::value;
};
template <int L, int M, int R, int N>
struct Search<false, false, L, M, R, N, true, true> {
    static const int value =
        Search<(R - L <= 1), ((R + M) / 2) * ((R + M) / 2) >= N, M, (R + M) / 2, R, N>::value;
};
template <int L, int M, int R, int N>
struct Search<false, false, L, M, R, N, true, false> {
    static const int value = Search<(R - L <= 1), true, M, (R + M) / 2, R, N>::value;
};

template <int N, bool = (N / 2 <= 46340)>
struct Sqrt {
    static const int value =
        Search<N <= 1, ((N + 1) / 2) * ((N + 1) / 2) >= N, 1, (N + 1) / 2, N, N>::value;
};
template <int N>
struct Sqrt<N, false> {
    static const int value = Search<N <= 1, true, 1, (N + 1) / 2, N, N>::value;
};

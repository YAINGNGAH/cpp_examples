#pragma once

#include <array>
template <int N>
constexpr std::array<std::array<int, N - 1>, N - 1> submatrix(
    const std::array<std::array<int, N>, N>& a, int row, int column) {
    std::array<std::array<int, N - 1>, N - 1> submx{};
    int i = 0;
    int j = 0;
    for (int inew = 0; inew < N - 1; ++inew) {
        if (inew == row) {
            i++;
        }
        j = 0;
        for (int jnew = 0; jnew < N - 1; ++jnew) {
            if (jnew == column) {
                j++;
            }
            (&std::get<0>((&std::get<0>(submx))[inew]))[jnew] = a[i][j];
            j++;
        }
        i++;
    }
    return submx;
}

template <int N>
constexpr int determinant(const std::array<std::array<int, N>, N>& a) {
    int det = 0;
    for (int row = 0; row < N; ++row) {
        det += (row % 2 == 0 ? 1 : -1) * a[row][0] * determinant<N - 1>(submatrix<N>(a, row, 0));
    }
    return det;
}
template <>
constexpr int determinant<1>(const std::array<std::array<int, 1>, 1>& a) {
    return a[0][0];
}
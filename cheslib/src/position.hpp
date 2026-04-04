#pragma once

#include <cstdint>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "piece_bitboards.hpp"
#include "state.hpp"

namespace cheslib {

class Position {
  public:
    constexpr Position(const PieceBitboards &pieces, State state) : _pieces(pieces), _state(state) {
    }

    static constexpr Position initial() {
        return Position(PieceBitboards::initial(), State::initial());
    }

    MoveList generate_pseudo_legals() const;

  private:
    template <bool IsBlack>
    void generate_pawn_moves(MoveList &moves) const;

    template <bool IsBlack>
    void generate_non_pawn_moves(MoveList &moves) const;

    template <bool IsBlack>
    void generate_castling_moves(MoveList &moves) const;

  private:
    PieceBitboards _pieces;
    State _state;
};

} // namespace cheslib

#pragma once

#include <bit>
#include <cstdint>
#include <vector>

#include "cheslib/attack_tables.hpp"
#include "cheslib/board.hpp"
#include "cheslib/constants.hpp"
#include "cheslib/move.hpp"

namespace cheslib {

class MoveGenerator {
  public:
    // Thường sử dụng một mảng tĩnh tránh cấp phát động (std::vector) để tối ưu mem
    struct MoveList {
        std::array<Move, 256> moves; // Số nước đi tối đa trong 1 position khoảng 218
        int count = 0;

        void add(Move m) {
            moves[count++] = m;
        }
    };

    // moves that doesn't consider King safety
    static MoveList generate_pseudo_moves(const Board &board, bool active_color_is_white) {
        MoveList list;
        generate_pawn_moves(list, board, active_color_is_white);
        generate_knight_moves(list, board, active_color_is_white);
        generate_sliding_moves(list, board, active_color_is_white);
        generate_king_moves(list, board, active_color_is_white);
        return list;
    }

  private:
    static void generate_knight_moves(MoveList &list, const Board &board, bool is_white) {
        uint64_t knights = board.get_pieces(is_white ? Color::WHITE : Color::BLACK, PieceType::KNIGHT);
        uint64_t friendly_pieces = board.get_occupancy(is_white ? Color::WHITE : Color::BLACK);

        while (knights) {
            int sq = std::countr_zero(knights);                       // Quét vị trí bit 1 thấp nhất (C++20)
            uint64_t attacks = knight_attacks(sq) & ~friendly_pieces; // Không đi vào ô có quân mình

            while (attacks) {
                int target_sq = std::countr_zero(attacks);
                list.add(Move(sq, target_sq));
                attacks &= attacks - 1; // Xóa bit thấp nhất
            }
            knights &= knights - 1; // Xóa bit hiệp sĩ vừa xử lý
        }
    }

    static void generate_sliding_moves(MoveList &list, const Board &board, bool is_white) {
        uint64_t rooks = board.get_pieces(is_white ? Color::WHITE : Color::BLACK, PieceType::ROOK);
        uint64_t all_occupancy = board.get_occupancy();
        uint64_t friendly_pieces = board.get_occupancy(is_white ? Color::WHITE : Color::BLACK);

        while (rooks) {
            int sq = std::countr_zero(rooks);
            // Sử dụng attack_tables bạn đã định nghĩa
            uint64_t attacks = rook_attacks(sq, all_occupancy) & ~friendly_pieces;

            while (attacks) {
                int target_sq = std::countr_zero(attacks);
                list.add(Move(sq, target_sq));
                attacks &= attacks - 1;
            }
            rooks &= rooks - 1;
        }

        // Tương tự cho Tượng (Bishops) và Hậu (Queens)
    }

    static void generate_pawn_moves(MoveList &list, const Board &board, bool is_white) {
        // Cần xử lý đẩy thẳng 1 ô, đẩy 2 ô, ăn quân chéo, phong cấp, và bắt tốt qua đường (En Passant)
    }

    static void generate_king_moves(MoveList &list, const Board &board, bool is_white) {
        // Giống Mã (Knight) nhưng ngoài ra phải xử lý nhập thành (Castling)
    }
};

} // namespace cheslib
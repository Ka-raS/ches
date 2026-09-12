#pragma once

#include "window.hpp"

#include <variant>

#include <cheslib/engine.hpp>

namespace ches {

struct GameContext;
struct StateResult;

namespace state {

class SelectingPiece {
  public:
    SelectingPiece(float previous_search_time = 0);
    StateResult update(GameContext &context, MouseEvent mouse) const;
    void draw(const GameContext &context) const;

  private:
    float _previous_search_time;
};

class DraggingPiece {
  public:
    DraggingPiece(::Vector2 mouse_position, cheslib::Square selected_piece);
    StateResult update(GameContext &context, MouseEvent mouse) const;
    void draw(const GameContext &context) const;

  private:
    ::Vector2 _mouse_position;
    cheslib::Square _selected_piece;
};

class SelectingDestination {
  public:
    SelectingDestination(cheslib::Square selected_piece);
    StateResult update(GameContext &context, MouseEvent mouse) const;
    void draw(const GameContext &context) const;

  private:
    cheslib::Square _selected_piece;
};

class PromotingPawn {
  public:
    PromotingPawn(cheslib::Square selected_pawn, cheslib::Square promotion_square);
    StateResult update(GameContext &context, MouseEvent mouse) const;
    void draw(const GameContext &context) const;

  private:
    cheslib::Square _selected_pawn;
    cheslib::Square _promotion_square;
};

class EnginePlaying {
  public:
    EnginePlaying(cheslib::Engine &engine);
    StateResult update(GameContext &context, MouseEvent mouse) const;
    void draw(const GameContext &context) const;

  private:
    float _start_time;
};

class GameOver {
  public:
    GameOver(cheslib::ChessStatus result);
    StateResult update(GameContext &context, MouseEvent mouse) const;
    void draw(const GameContext &context) const;

  private:
    cheslib::ChessStatus _result;
};

} // namespace state

// clang-format off

using GameState = std::variant<
    state::SelectingPiece,
    state::DraggingPiece,
    state::SelectingDestination,
    state::PromotingPawn,
    state::EnginePlaying,
    state::GameOver
>;
// clang-format on

static_assert(sizeof(GameState) == 16); // fun fact

struct StateResult {
    GameState next_state;
    ::MouseCursor cursor = ::MOUSE_CURSOR_DEFAULT;
};

} // namespace ches

// ============================================================
//  StealthOps: The Heist  (browser version)
//  Game logic mirrors the C++ version in src/code.cpp
// ============================================================

const MAP_TEMPLATE = [
  "############",
  "#....#....E#",
  "#.#..#.#...#",
  "#.#.....#..#",
  "#..........#",
  "###.###.#..#",
  "#...#...#..#",
  "#....#..T..#",
  "#..........#",
  "############",
];

const MAX_MOVES = 35;

// ---------- DOM ----------
const boardEl = document.getElementById("board");
const movesEl = document.getElementById("moves");
const movesBarEl = document.getElementById("moves-bar");
const treasureEl = document.getElementById("treasure");
const messageEl = document.getElementById("message");
const overlayEl = document.getElementById("overlay");
const overlayTitleEl = document.getElementById("overlay-title");
const overlayTextEl = document.getElementById("overlay-text");

let state;

// ---------- Game setup ----------
function newGame() {
  state = {
    map: MAP_TEMPLATE.map((row) => row.split("")),
    player: { x: 1, y: 1, hasTreasure: false },
    guard: { x: 4, y: 4, minX: 3, maxX: 9, dir: 1 },
    camera: { x: 1, y: 7 },
    movesLeft: MAX_MOVES,
    over: false,
    won: false,
  };
  overlayEl.classList.add("hidden");
  setMessage("Use W A S D or the arrow keys to move.");
  render();
}

// ---------- Helpers ----------
function isWall(x, y) {
  return state.map[y][x] === "#";
}

function setMessage(text, type = "") {
  messageEl.textContent = text;
  messageEl.className = "message " + type;
}

// Same logic as SecurityCamera::detectsPlayer in C++
function cameraDetectsPlayer() {
  const { camera, player, map } = state;
  if (player.y !== camera.y || player.x === camera.x) return false;

  const step = player.x > camera.x ? 1 : -1;
  for (let col = camera.x + step; col !== player.x + step; col += step) {
    if (map[camera.y][col] === "#") return false;
  }
  return true;
}

// Cells the camera can currently see (used for the red highlight)
function cameraVisibleCells() {
  const { camera, map } = state;
  const cells = new Set();
  for (const step of [1, -1]) {
    let col = camera.x + step;
    while (map[camera.y][col] !== "#") {
      cells.add(`${col},${camera.y}`);
      col += step;
    }
  }
  return cells;
}

// ---------- Game rules ----------
function checkTreasure() {
  const { player, map } = state;
  if (map[player.y][player.x] === "T") {
    player.hasTreasure = true;
    map[player.y][player.x] = ".";
    setMessage("💎 Treasure collected! Now find the exit.", "good");
  }
}

function checkExit() {
  const { player, map } = state;
  if (map[player.y][player.x] === "E") {
    if (player.hasTreasure) {
      state.won = true;
      state.over = true;
    } else {
      setMessage("🔒 Exit locked! Collect the treasure first.", "bad");
    }
  }
}

function moveGuard() {
  const g = state.guard;
  g.x += g.dir;
  if (g.x >= g.maxX) {
    g.x = g.maxX;
    g.dir = -1;
  } else if (g.x <= g.minX) {
    g.x = g.minX;
    g.dir = 1;
  }
}

function checkDetection() {
  const { player, guard } = state;
  const distance = Math.abs(player.x - guard.x) + Math.abs(player.y - guard.y);

  if (distance <= 1) {
    state.over = true;
    state.won = false;
    setMessage("💂 The guard caught you! Mission failed.", "bad");
    return "guard";
  }
  if (cameraDetectsPlayer()) {
    state.over = true;
    state.won = false;
    setMessage("📷 The security camera spotted you! Mission failed.", "bad");
    return "camera";
  }
  return null;
}

// ---------- One turn ----------
function takeTurn(dir) {
  if (state.over) return;

  const { player } = state;
  let nx = player.x;
  let ny = player.y;
  if (dir === "w") ny--;
  else if (dir === "s") ny++;
  else if (dir === "a") nx--;
  else if (dir === "d") nx++;

  // Wall: block the move, no move used
  if (isWall(nx, ny)) {
    setMessage("🧱 A wall blocks your path!", "bad");
    return;
  }

  player.x = nx;
  player.y = ny;
  state.movesLeft--;
  setMessage("Sneaking...");

  checkTreasure();
  checkExit();

  let reason = null;
  if (!state.over) {
    moveGuard();
    reason = checkDetection();
  }

  if (!state.over && state.movesLeft === 0) {
    state.over = true;
    state.won = false;
    setMessage("⏳ You ran out of moves.", "bad");
    reason = "moves";
  }

  render(reason);

  if (state.over) showEnding(reason);
}

// ---------- Rendering ----------
function render(reason = null) {
  const { map, player, guard, camera } = state;
  const seen = cameraVisibleCells();

  boardEl.innerHTML = "";

  for (let y = 0; y < map.length; y++) {
    for (let x = 0; x < map[y].length; x++) {
      const cell = document.createElement("div");
      cell.className = "cell";
      let sprite = "";
      const tile = map[y][x];

      if (tile === "#") cell.classList.add("wall");
      if (seen.has(`${x},${y}`)) cell.classList.add("seen");

      if (tile === "T") {
        cell.classList.add("treasure");
        sprite = "💎";
      }
      if (tile === "E") {
        cell.classList.add("exit", player.hasTreasure ? "open" : "locked");
        sprite = "🚪";
      }
      if (x === camera.x && y === camera.y) sprite = "📷";
      if (x === guard.x && y === guard.y) {
        sprite = "💂";
        cell.classList.add("guard");
        if (reason === "guard") cell.classList.add("danger");
      }
      if (x === player.x && y === player.y) sprite = "🥷";

      if (sprite) {
        const span = document.createElement("span");
        span.className = "sprite";
        span.textContent = sprite;
        cell.appendChild(span);
      }
      boardEl.appendChild(cell);
    }
  }

  // HUD
  movesEl.textContent = state.movesLeft;
  const pct = (state.movesLeft / MAX_MOVES) * 100;
  movesBarEl.style.width = pct + "%";
  movesBarEl.classList.toggle("low", pct <= 25);

  treasureEl.textContent = player.hasTreasure ? "💎 Collected" : "Not collected";
  treasureEl.classList.toggle("got", player.hasTreasure);
}

function showEnding(reason) {
  overlayTitleEl.className = state.won ? "win" : "lose";

  if (state.won) {
    overlayTitleEl.textContent = "MISSION SUCCESS!";
    overlayTextEl.textContent =
      "You collected the treasure and escaped with " + state.movesLeft + " moves to spare.";
    setMessage("🎉 You escaped with the treasure!", "win");
  } else if (reason === "guard") {
    overlayTitleEl.textContent = "CAUGHT!";
    overlayTextEl.textContent = "The guard caught you. Time your moves around his patrol.";
  } else if (reason === "camera") {
    overlayTitleEl.textContent = "SPOTTED!";
    overlayTextEl.textContent = "The camera saw you. Stay out of its line of sight.";
  } else {
    overlayTitleEl.textContent = "OUT OF MOVES";
    overlayTextEl.textContent = "You ran out of moves before escaping.";
  }

  // small delay so the final frame is visible
  setTimeout(() => overlayEl.classList.remove("hidden"), 350);
}

// ---------- Input ----------
const KEY_MAP = {
  w: "w", arrowup: "w",
  s: "s", arrowdown: "s",
  a: "a", arrowleft: "a",
  d: "d", arrowright: "d",
};

document.addEventListener("keydown", (e) => {
  const key = e.key.toLowerCase();

  if (key === "r") {
    newGame();
    return;
  }
  if (KEY_MAP[key]) {
    e.preventDefault();
    takeTurn(KEY_MAP[key]);
  }
});

document.querySelectorAll(".key").forEach((btn) => {
  btn.addEventListener("click", () => takeTurn(btn.dataset.dir));
});

document.getElementById("restart").addEventListener("click", newGame);
document.getElementById("overlay-btn").addEventListener("click", newGame);

// ---------- Start ----------
newGame();

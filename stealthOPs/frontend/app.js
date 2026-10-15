const originalMap = [
  "############",
  "#....#....E#",
  "#.#..#.#...#",
  "#.#.....#..#",
  "#..........#",
  "###.###.#..#",
  "#...#...#..#",
  "#....#..T..#",
  "#..........#",
  "############"
];

let map, player, guard, camera, movesLeft, hasTreasure, gameOver, won, logEntries;
const $ = (id) => document.getElementById(id);

function resetGame() {
  map = originalMap.map(row => row.split(""));
  player = { x: 1, y: 1 };
  guard = { x: 4, y: 4, minX: 3, maxX: 9, direction: 1 };
  camera = { x: 1, y: 7 };
  movesLeft = 35;
  hasTreasure = false;
  gameOver = false;
  won = false;
  logEntries = [];
  $("resultModal").classList.add("hidden");
  addLog("Mission started. Recover the intel and reach extraction.");
  render();
}

function addLog(message, alert = false) {
  const now = new Date();
  logEntries.unshift({ time: now.toLocaleTimeString([], {hour:"2-digit", minute:"2-digit"}), message, alert });
  $("missionLog").innerHTML = logEntries.slice(0, 8).map(e =>
    `<div class="log-entry ${e.alert ? "alert" : ""}"><span class="log-time">${e.time}</span><span>${escapeHtml(e.message)}</span></div>`
  ).join("");
  $("logCount").textContent = `${String(logEntries.length).padStart(2, "0")} ENTRIES`;
}
function escapeHtml(text) {
  return text.replace(/[&<>"']/g, c => ({"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;","'":"&#39;"}[c]));
}
function isWall(x, y) {
  return y < 0 || y >= map.length || x < 0 || x >= map[y].length || map[y][x] === "#";
}
function render() {
  const board = map.map(row => row.slice());
  board[camera.y][camera.x] = "C";
  board[guard.y][guard.x] = "G";
  board[player.y][player.x] = "P";
  const grid = $("gameMap");
  grid.innerHTML = "";
  board.forEach((row, y) => row.forEach((symbol, x) => {
    const cell = document.createElement("div");
    cell.className = "cell";
    cell.setAttribute("role", "gridcell");
    cell.setAttribute("aria-label", `Row ${y+1}, column ${x+1}: ${symbol}`);
    if (symbol === "#") cell.classList.add("wall");
    else if (symbol === "P") { cell.classList.add("player"); cell.textContent = "P"; }
    else if (symbol === "G") { cell.classList.add("guard"); cell.textContent = "G"; }
    else if (symbol === "C") { cell.classList.add("camera"); cell.textContent = "C"; }
    else if (symbol === "T") { cell.classList.add("treasure"); cell.textContent = "◆"; }
    else if (symbol === "E") { cell.classList.add("exit"); cell.textContent = "↗"; }
    else cell.classList.add("path");
    grid.appendChild(cell);
  }));
  $("movesLeft").textContent = movesLeft;
  $("intelStatus").textContent = hasTreasure ? "SECURED" : "NOT SECURED";
  $("playerPosition").textContent = `${String(player.x+1).padStart(2,"0")} : ${String(player.y+1).padStart(2,"0")}`;
  $("threatLevel").textContent = gameOver ? (won ? "CLEAR" : "CRITICAL") : (movesLeft < 10 ? "HIGH" : "LOW");
  $("threatLevel").style.color = gameOver && won ? "var(--green)" : movesLeft < 10 || gameOver ? "var(--red)" : "var(--green)";
  $("intelObjective").classList.toggle("complete", hasTreasure);
  $("intelObjective").querySelector(".objective-state").textContent = hasTreasure ? "COMPLETE" : "PENDING";
  $("exitObjective").classList.toggle("complete", won);
  $("exitObjective").querySelector(".objective-state").textContent = won ? "COMPLETE" : hasTreasure ? "UNLOCKED" : "LOCKED";
}
function cameraSeesPlayer() {
  if (player.y !== camera.y || player.x === camera.x) return false;
  const step = player.x > camera.x ? 1 : -1;
  for (let col = camera.x + step; col !== player.x + step; col += step) {
    if (map[camera.y][col] === "#") return false;
  }
  return true;
}
function finish(success, message) {
  gameOver = true;
  won = success;
  render();
  addLog(message, !success);
  $("resultTitle").textContent = success ? "MISSION SUCCESS" : "MISSION FAILED";
  $("resultText").textContent = `${message} Moves used: ${35 - movesLeft}. Intel: ${hasTreasure ? "secured" : "not secured"}. This browser prototype does not write to the C++ history file.`;
  $("resultModal").classList.remove("hidden");
}
function movePlayer(direction) {
  if (gameOver) return;
  const deltas = { w:[0,-1], s:[0,1], a:[-1,0], d:[1,0] };
  if (!deltas[direction]) return;
  const [dx,dy] = deltas[direction];
  const nx = player.x + dx, ny = player.y + dy;
  if (isWall(nx,ny)) {
    addLog("A wall blocks your path.");
    return;
  }
  player.x = nx; player.y = ny; movesLeft--;
  addLog(`Moved to column ${player.x+1}, row ${player.y+1}.`);
  if (map[player.y][player.x] === "T") {
    hasTreasure = true;
    map[player.y][player.x] = ".";
    addLog("Intel recovered. Extraction is now unlocked.");
  }
  if (map[player.y][player.x] === "E") {
    if (hasTreasure) { finish(true, "Intel secured. Extraction successful."); return; }
    addLog("Extraction is locked. Recover the intel first.", true);
  }
  guard.x += guard.direction;
  if (guard.x >= guard.maxX) { guard.x = guard.maxX; guard.direction = -1; }
  else if (guard.x <= guard.minX) { guard.x = guard.minX; guard.direction = 1; }
  const distance = Math.abs(player.x - guard.x) + Math.abs(player.y - guard.y);
  if (distance <= 1) { finish(false, "The guard caught you. Mission compromised."); return; }
  if (cameraSeesPlayer()) { finish(false, "The security camera spotted you. Mission compromised."); return; }
  if (movesLeft <= 0) { finish(false, "You ran out of moves before extraction."); return; }
  render();
}
document.querySelectorAll("[data-move]").forEach(btn => btn.addEventListener("click", () => movePlayer(btn.dataset.move)));
document.addEventListener("keydown", e => {
  if (["ArrowUp","ArrowDown","ArrowLeft","ArrowRight"].includes(e.key)) e.preventDefault();
  const key = ({ArrowUp:"w",ArrowDown:"s",ArrowLeft:"a",ArrowRight:"d"})[e.key] || e.key.toLowerCase();
  if ("wasd".includes(key) && key.length === 1) movePlayer(key);
});
$("restartBtn").addEventListener("click", resetGame);
$("modalRestart").addEventListener("click", resetGame);
$("quitBtn").addEventListener("click", () => { if (!gameOver) finish(false, "Mission aborted by operator."); });
resetGame();

// ui.js — intro screen, parameter panel, and WASM bridge

(function () {
  const panel = document.getElementById('panel');

  // ─── Screen 1: Intro ───────────────────────────────────────────────────────
  function showIntro() {
    panel.innerHTML = `
      <h2>GAME OF LIFE SVG</h2>

      <p class="intro-text">
        Conway's Game of Life was invented by mathematician John Conway in 1970.
        It is a zero-player simulation: you set the initial state, then watch
        emergent complexity arise from four simple rules about birth, survival,
        and death.
      </p>

      <p class="intro-text">
        This version treats cells as living organisms. Instead of plain squares,
        each cell is drawn based on how long it has survived.
      </p>

      <div class="legend">
        <div class="legend-item">
          <span class="dot infant"></span>
          <span><strong>Infant</strong> (1–5 gen) — pulsing dot. Newborn, unstable.</span>
        </div>
        <div class="legend-item">
          <span class="dot adult"></span>
          <span><strong>Adult</strong> (6–20 gen) — polygon. Shape reflects neighbor count.</span>
        </div>
        <div class="legend-item">
          <span class="dot ancient"></span>
          <span><strong>Ancient</strong> (21+ gen) — polar flower. Rare, stable, complex.</span>
        </div>
      </div>

      <button id="next-btn">CONFIGURE →</button>
    `;

    document.getElementById('next-btn').addEventListener('click', showParams);
  }

  // ─── Screen 2: Parameters ──────────────────────────────────────────────────
  function showParams() {
    panel.innerHTML = `
      <h2>GAME OF LIFE SVG</h2>

      <div class="param">
        <label>Grid Size</label>
        <input type="range" id="grid-size" min="32" max="256" step="32" value="128"
               oninput="document.getElementById('grid-size-val').textContent = this.value" />
        <span class="value" id="grid-size-val">128</span>
      </div>

      <div class="param">
        <label>Cell Density</label>
        <input type="range" id="density" min="0.1" max="0.9" step="0.05" value="0.3"
               oninput="document.getElementById('density-val').textContent = parseFloat(this.value).toFixed(2)" />
        <span class="value" id="density-val">0.30</span>
      </div>

      <div class="param">
        <label>Speed (ms)</label>
        <input type="range" id="speed" min="50" max="500" step="50" value="100"
               oninput="document.getElementById('speed-val').textContent = this.value" />
        <span class="value" id="speed-val">100</span>
      </div>

      <div class="param">
        <label>Pattern</label>
        <select id="pattern">
          <option value="0">Random</option>
          <option value="1">Glider</option>
          <option value="2">Blinker</option>
          <option value="3">R-Pentomino</option>
        </select>
      </div>

      <button id="start-btn">START</button>
    `;

    document.getElementById('start-btn').addEventListener('click', function () {
      const gridSize = parseInt(document.getElementById('grid-size').value);
      const density  = parseFloat(document.getElementById('density').value);
      const speed    = parseInt(document.getElementById('speed').value);
      const pattern  = parseInt(document.getElementById('pattern').value);

      // Hide panel, show canvas
      panel.style.display = 'none';
      document.getElementById('gooey-container').style.display = 'block';

      // Wait for WASM module to be ready, then call init
      function tryInit() {
        if (typeof Module !== 'undefined' && Module.ccall) {
          Module.ccall('gol_init', null,
            ['number', 'number', 'number', 'number'],
            [gridSize, density, speed, pattern]
          );
        } else {
          setTimeout(tryInit, 50);
        }
      }

      tryInit();
    });
  }

  // Start on intro screen
  showIntro();
})();
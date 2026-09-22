# GEN UI — CSS Lite (autoría XML+CSS)

Guía corta para autores y el backlog post–Opción A (UIScale).  
Validación: `Examples/Graphics/UI_System` + `UnitTests_UserInterface`.

Contrato dual: layouts **sin** stylesheet (`UI_Options`) no cambian. Solo layouts con CSS / scale activo.

---

## Contrato de autoría

| En CSS (con stylesheet) | En XML (o aún no en CSS) |
|-------------------------|---------------------------|
| color, background-color / bckgrdcolor | Widgets tipados: level, thickness, gradient*, imageempty/full, offset*, startangle… |
| margin / padding (+ longhands), border-*, border-radius, box-shadow | |
| xpos, ypos, width, height (+ % / rem / vw / vh / em / calc en carga) | |
| display: flex \| grid, flex-*, gap (+ rem), align-*, justify-*, grid-template-*, grid-*-span | |
| text-align / textalignment | |
| sizefont / font-size (Text, TextBox vía cascade) | font-family (un vector por skin) |
| transition (ms), :hover / :selected / … | |
| @import, :root variables, var(--x) en reglas de hoja | var() **no** se expande en `style=""` inline (usar literales) |

**Paint (política Track D.3):**

- **Fill + stroke border + radius:** `Draw_Form` (y menús que delegan en form). Otros tipos pueden **parsear** border-* sin dibujarlos.
- **box-shadow:** form (AABB soft/hard); image / animation vía `DrawElementBoxShadow`; progressradial = halo anular (no disco relleno).
- **padding:** solo inset de layout para hijos; no se “pinta” como marco.

**Unidades (Fase 8):** rem/vw/vh se resuelven en `GetLayoutElement_Base` al cargar. Viewport = design size; rem = `--root-font-size` en `:root` (default 16). No hay re-resolve automático en resize (load-time); ver Track L.3.

**Scale:** autores escriben en **design px**. `UIScale` escala paint/input; no reescribir XML por resolución.

---

## Tracks (backlog)

### Track D — Consolida CSS Lite (ciclo actual)

| # | Paso | Estado |
|---|------|--------|
| D.1 | Esta guía + SCOPE `dashboard.css` alineado al motor | ✅ |
| D.2 | Tipografía demo vía CSS (`sizefont` / `font-size`) | ✅ |
| D.3 | Política paint documentada (arriba) | ✅ |
| D.4 | Inline `style=`: var() explícitamente no soportado | ✅ documentado |

### Track L — Layout

| # | Paso | Estado |
|---|------|--------|
| L.1 | Demo Grid real en UI_System (fila superior de cards) | ✅ `form.cards-grid` |
| L.2 | rem en gap / flex-basis | ✅ gap cableado; demo `row-gap`/`column-gap` rem |
| L.3 | vw/vh en resize | ✅ **cerrado: load-time only** (no re-resolve al resize; ver nota abajo) |
| L.4 | RunLayout re-evalúa lengths | no aplica mientras L.3 = load-time |

**L.3 (cerrado):** rem/vw/vh/% se resuelven una vez en `GetLayoutElement_Base`. Al resize/autofit solo cambia `UIScale` (paint/input), no se re-parsea CSS. Autores que necesiten longitudes que sigan el viewport en caliente deberán esperar un futuro re-resolve (L.4) o usar scale.

### Track P — Android 5.3 + smoke

| # | Paso | Estado |
|---|------|--------|
| P.1 | Contrato Android: design canvas = 1440×900; fit visual = GLES letterbox; UIScale = 1.0 | ✅ UI_System fuerza scale 1.0 (`#ifdef ANDROID`) |
| P.2 | Config change: no destruir design size con UpdateSize(native) | ✅ `MainProcANDROID` GLES: keep design + `CHANGESIZE` |
| P.3 | Build `android_apk` (arm64) + checklist dispositivo/emulador | ✅ checklist `captures/uiscale/PP_android_notes.md` |

**Contrato P:** en Android el canvas de autoría permanece 1440×900; el letterbox GLES + `MapWindowToCanvas` adaptan a la superficie nativa. `UIScale` Present = 1.0 (sin doble escala). `OnConfigurationChanged` no llama `UpdateSize(native)` bajo GLES.

### Track B — `@media` / breakpoints

Diferido (después de Track P / Nivel 4).

### Track Q — Calidad

Diagnóstico de parseo CSS, capturas, suite verde en cada PR.

---

## Puerta de producto (post D+L.1)

| Si necesitáis… | Elegid |
|----------------|--------|
| Reflow por ancho (móvil / ventana estrecha) | Track B |
| Mismo dashboard en Android/DPI | Track P |
| Ninguno urgente | **Parar en Nivel 1** (producto dashboard) o seguir L.2 |

**Decisión fijada:** **Nivel 2** alcanzado (L.3 = load-time). **Track P** cerrado → camino a **Nivel 4**. Track B sigue diferido.

Capturas: `PD_L1_*`, `PL2_gap_rem.png`, checklist Android `PP_android_*`.

---

## Definición de hecho (niveles)

| Nivel | Significa |
|-------|-----------|
| 1 — Producto dashboard | Opción A + Track D + L.1 |
| 2 — Layout expresivo | Nivel 1 + L.2 + L.3 explícito |
| 3 — Responsive | Nivel 2 + Track B |
| 4 — Multiplataforma | Nivel 1+ + Track P |

Opción A (scale): ver [GEN_UI_Scale_Roadmap.md](GEN_UI_Scale_Roadmap.md).

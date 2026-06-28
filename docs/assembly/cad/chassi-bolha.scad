/*
Projeto: Chassi + bolha para carro autônomo Arduino Mega 2WD
Versão: v2 - correção de alinhamento do tray + bolha estilo sedã premium (inspirada no Mercedes C300)
Autor: gerado por ChatGPT
Unidades: milímetros
Idioma: PT-BR

Instruções:
1. Abra este arquivo no OpenSCAD.
2. Altere a variável `part` para exportar uma peça por vez.
3. Pressione F6 para renderizar.
4. Exporte STL em File > Export > Export as STL.

Opções de `part`:
- "base"       : placa base estrutural
- "tray"       : berço/nichos dos eletrônicos (corrigido)
- "husky"      : torre/suporte HuskyLens
- "hc_sr04"    : suporte frontal HC-SR04
- "line_ir"    : suporte inferior do array IR de linha
- "bubble"     : bolha/carroceria superior estilo sedã premium
- "assembly"   : conjunto completo de referência
*/

part = "assembly";

// ==========================================================
// PARÂMETROS GERAIS DO VEÍCULO
// ==========================================================

base_len = 245;
base_w   = 155;
base_t   = 3;
corner_r = 12;

wheel_d = 66;
wheel_w = 25;
wheel_center_y = 90;

rear_axle_x  = 47;
front_axle_x = 200;

base_clearance = 38;               // distância do solo até a face inferior da base
base_top_z = base_t;               // em peças exportadas, base começa em z=0

// Folgas
fit_clear = 1.2;
wall_t = 2.4;

// ==========================================================
// PARÂMETROS DOS COMPONENTES / LAYOUT REVISADO
// Sistema de coordenadas das peças exportadas:
// x = 0 mm na traseira da placa
// x = base_len na frente da placa
// y = 0 mm no eixo central longitudinal
// z = 0 mm na face inferior da peça exportada
// ==========================================================

// Powerbank: centralizado e próximo ao eixo traseiro.
pbank_cx = 65;
pbank_cy = 0;
pbank_l  = 92;
pbank_w  = 52;
pbank_h  = 46;

// Arduino Mega 2560: mantido no quadrante dianteiro esquerdo.
mega_cx = 169;
mega_cy = -38;
mega_l  = 107;
mega_w  = 58;

// L298N: quadrante dianteiro direito intermediário.
l298_cx = 140;
l298_cy = 42;
l298_l  = 50;
l298_w  = 50;
l298_h  = 27;

// Pack 2S 18650: deslocado levemente para dentro para eliminar sobreposição/área vazada no tray.
pack_cx = 198;
pack_cy = 44;
pack_l  = 82;
pack_w  = 46;
pack_h  = 24;

// Array IR de linha.
line_ir_x = 226;
line_ir_spacing = 44;

// Bolha.
bubble_wall = 2.0;
bubble_flange_t = 3.0;
bubble_mount_hole_d = 3.4;

// Alturas relativas ao SOLO em montagem final (referência documental)
hc_height_ground     = 62;
ir_obs_height_ground = 52;
husky_lens_ground    = 118;

// Alturas relativas ao topo da base para montagem geométrica
hc_z_local     = 24;
ir_obs_z_local = 14;
husky_z_local  = 80;

// Furos de fixação da bolha na placa.
body_mount_pts = [
    [18,  65],
    [18, -65],
    [120, 70],
    [120,-70],
    [228, 65],
    [228,-65]
];

// Furos aproximados do Arduino Mega, relativos ao canto inferior esquerdo do envelope do nicho.
mega_holes_local = [
    [14,  3],
    [15, 51],
    [66,  8],
    [91, 51]
];

// ==========================================================
// MÓDULOS AUXILIARES
// ==========================================================

module rounded_rect_2d(l, w, r) {
    hull() {
        translate([ l/2-r,  w/2-r]) circle(r=r, $fn=32);
        translate([-l/2+r,  w/2-r]) circle(r=r, $fn=32);
        translate([ l/2-r, -w/2+r]) circle(r=r, $fn=32);
        translate([-l/2+r, -w/2+r]) circle(r=r, $fn=32);
    }
}

module panel_xy(l, w, t, r=8) {
    linear_extrude(height=t)
        rounded_rect_2d(l, w, r);
}

module m3_hole(h=10) {
    cylinder(h=h, d=3.4, center=true, $fn=24);
}

module slot_2d(len, dia) {
    hull() {
        translate([-(len-dia)/2, 0]) circle(d=dia, $fn=24);
        translate([ (len-dia)/2, 0]) circle(d=dia, $fn=24);
    }
}

module slot_cut_z(len, dia, h=10) {
    linear_extrude(height=h, center=true)
        slot_2d(len, dia);
}

module simple_standoff(h=8, od=7, hole_d=3.4) {
    difference() {
        cylinder(h=h, d=od, $fn=32);
        translate([0,0,-1]) cylinder(h=h+2, d=hole_d, $fn=24);
    }
}

module tray_nest(outer_l, outer_w, inner_l, inner_w, wall=2.4, h=10, open_side="none") {
    // Caixa com fundo, pensada para imprimir deitada.
    difference() {
        union() {
            cube([outer_l, outer_w, h], center=true);
        }
        translate([0,0,wall])
            cube([inner_l, inner_w, h+1], center=true);

        // aberturas laterais opcionais
        if (open_side == "left") {
            translate([0, -outer_w/2 + wall/2, h/2]) cube([outer_l + 2, wall + 1.5, h + 2], center=true);
        }
        if (open_side == "right") {
            translate([0,  outer_w/2 - wall/2, h/2]) cube([outer_l + 2, wall + 1.5, h + 2], center=true);
        }
        if (open_side == "rear") {
            translate([-outer_l/2 + wall/2, 0, h/2]) cube([wall + 1.5, outer_w + 2, h + 2], center=true);
        }
        if (open_side == "front") {
            translate([ outer_l/2 - wall/2, 0, h/2]) cube([wall + 1.5, outer_w + 2, h + 2], center=true);
        }
    }
}

module ring_stops(l, w, stop_d=6, stop_h=4, inset=5) {
    for (sx = [-1,1])
        for (sy = [-1,1])
            translate([sx*(l/2-inset), sy*(w/2-inset), stop_h/2])
                cylinder(h=stop_h, d=stop_d, center=true, $fn=24);
}

// ==========================================================
// 1. PLACA BASE
// ==========================================================

module base_plate() {
    difference() {
        translate([base_len/2, 0, 0])
            panel_xy(base_len, base_w, base_t, corner_r);

        // Furos de fixação da bolha.
        for (p = body_mount_pts)
            translate([p[0], p[1], base_t/2])
                m3_hole(h=base_t+4);

        // Rasgos para cinta do powerbank.
        for (xpos = [35, 95]) {
            translate([xpos,  33, base_t/2]) rotate([0,0,90]) slot_cut_z(18, 4.2, base_t+4);
            translate([xpos, -33, base_t/2]) rotate([0,0,90]) slot_cut_z(18, 4.2, base_t+4);
        }

        // Rasgos para cinta do pack 2S.
        for (xpos = [pack_cx - 22, pack_cx + 22]) {
            translate([xpos, pack_cy + 28, base_t/2]) rotate([0,0,90]) slot_cut_z(16, 4.2, base_t+4);
            translate([xpos, pack_cy - 28, base_t/2]) rotate([0,0,90]) slot_cut_z(16, 4.2, base_t+4);
        }

        // Arduino Mega: rasgos M3 aproximados.
        for (p = mega_holes_local)
            let (hx = mega_cx - mega_l/2 + p[0], hy = mega_cy - mega_w/2 + p[1])
                translate([hx, hy, base_t/2]) slot_cut_z(8, 3.4, base_t+4);

        // L298N: furação 4 cantos, assumindo passo 37 mm.
        for (dx = [-18.5, 18.5])
            for (dy = [-18.5, 18.5])
                translate([l298_cx + dx, l298_cy + dy, base_t/2])
                    m3_hole(h=base_t+4);

        // Base da torre HuskyLens.
        for (dy = [-14, 14])
            translate([232, dy, base_t/2])
                rotate([0,0,90]) slot_cut_z(14, 3.4, base_t+4);

        // Suporte frontal HC-SR04.
        for (dy = [-20, 20])
            translate([238, dy, base_t/2])
                rotate([0,0,90]) slot_cut_z(12, 3.4, base_t+4);

        // Suporte do array de IR de linha.
        for (dy = [-55, 0, 55])
            translate([line_ir_x, dy, base_t/2])
                rotate([0,0,90]) slot_cut_z(16, 3.4, base_t+4);

        // Passagem de cabo de sinal.
        translate([178, -6, base_t/2]) slot_cut_z(72, 8, base_t+4);

        // Passagem de cabo de potência.
        translate([114, 18, base_t/2]) slot_cut_z(60, 8, base_t+4);
    }
}

// ==========================================================
// 2. BERÇO DOS ELETRÔNICOS (CORRIGIDO)
// Problema corrigido: o tray agora tem base com footprint compatível
// com a placa e todos os nichos ficaram inteiramente contidos na área.
// ==========================================================

module electronics_tray() {
    tray_plate_t = 2.4;
    tray_len = 241;     // quase toda a base, deixando apenas borda funcional
    tray_wid = 151;
    tray_corner = 10;

    // alturas das caixas
    h_low = 10;
    h_high = 14;

    union() {
        // base plena do tray, agora coincidente com a base e sem áreas vazadas
        translate([base_len/2, 0, 0])
            panel_xy(tray_len, tray_wid, tray_plate_t, tray_corner);

        // Nicho powerbank: centralizado, próximo ao eixo traseiro, lado esquerdo aberto.
        translate([pbank_cx, pbank_cy, tray_plate_t + h_high/2]) {
            tray_nest(pbank_l + 2*wall_t, pbank_w + 2*wall_t, pbank_l + fit_clear, pbank_w + fit_clear, wall_t, h_high, "left");
            ring_stops(pbank_l, pbank_w, stop_d=6, stop_h=4, inset=8);
        }

        // Nicho Mega: contido integralmente no tray.
        translate([mega_cx, mega_cy, tray_plate_t + h_low/2]) {
            tray_nest(mega_l + 2*wall_t, mega_w + 2*wall_t, mega_l + fit_clear, mega_w + fit_clear, wall_t, h_low, "left");
            for (p = mega_holes_local)
                let (hx = -mega_l/2 + p[0], hy = -mega_w/2 + p[1])
                    translate([hx, hy, 0]) simple_standoff(h=8, od=7, hole_d=3.4);
        }

        // Nicho L298N.
        translate([l298_cx, l298_cy, tray_plate_t + h_low/2]) {
            tray_nest(l298_l + 2*wall_t, l298_w + 2*wall_t, l298_l + fit_clear, l298_w + fit_clear, wall_t, h_low, "none");
            for (dx = [-18.5, 18.5])
                for (dy = [-18.5, 18.5])
                    translate([dx, dy, 0]) simple_standoff(h=6, od=7, hole_d=3.4);
        }

        // Nicho pack 2S: deslocado para dentro para não ultrapassar a borda da peça.
        translate([pack_cx, pack_cy, tray_plate_t + h_high/2]) {
            tray_nest(pack_l + 2*wall_t, pack_w + 2*wall_t, pack_l + fit_clear, pack_w + fit_clear, wall_t, h_high, "front");
            ring_stops(pack_l, pack_w, stop_d=6, stop_h=4, inset=8);
        }

        // Canaleta de sinal (lado esquerdo superior)
        translate([160, -8, tray_plate_t + 4]) cube([85, 5, 8], center=true);

        // Canaleta de potência (lado direito)
        translate([120, 20, tray_plate_t + 4]) cube([76, 5, 8], center=true);
    }
}

// ==========================================================
// 3. SUPORTE HUSKYLENS
// ==========================================================

module husky_mount() {
    base_l = 64;
    base_w2 = 35;
    base_h = 5;

    side_t = 4;
    side_h = 80;
    inner_w = 56;

    translate([232, 0, 0]) {
        union() {
            translate([0,0,base_h/2]) cube([base_l, base_w2, base_h], center=true);

            for (sy = [-1, 1]) {
                translate([5, sy*(inner_w/2 + side_t/2), base_h + side_h/2])
                    difference() {
                        cube([50, side_t, side_h], center=true);
                        translate([5,0,12]) rotate([90,0,0]) slot_cut_z(24, 3.4, side_t+2);
                        translate([-12,0,28]) rotate([90,0,0]) cylinder(h=side_t+2, d=3.4, center=true, $fn=24);
                    }
            }

            translate([5,0,base_h + side_h - 5]) cube([46, inner_w + 2*side_t, 6], center=true);
        }
    }
}

// ==========================================================
// 4. SUPORTE HC-SR04
// ==========================================================

module hc_sr04_mount() {
    plate_w = 56;
    plate_h = 30;
    plate_t = 3;

    translate([base_len + 2, 0, hc_z_local]) {
        rotate([0,90,0])
        difference() {
            cube([plate_w, plate_h, plate_t], center=true);
            for (dy = [-13, 13])
                translate([dy, 0, 0]) cylinder(h=plate_t+2, d=18.5, center=true, $fn=40);
            for (dy = [-23, 23])
                translate([dy, -10, 0]) slot_cut_z(10, 3.4, plate_t+2);
        }
    }
}

// ==========================================================
// 5. SUPORTE ARRAY IR DE LINHA
// ==========================================================

module line_ir_mount() {
    bar_l = 130;
    bar_w2 = 18;
    bar_t = 3;

    translate([line_ir_x, 0, -12]) {
        difference() {
            cube([bar_w2, bar_l, bar_t], center=true);
            for (sy = [-line_ir_spacing, 0, line_ir_spacing])
                translate([0, sy, 0]) rotate([0,0,90]) slot_cut_z(22, 2.8, bar_t+2);
        }

        for (sy = [-line_ir_spacing, 0, line_ir_spacing])
            translate([0, sy, -2]) cube([12, 50, 1], center=true);
    }
}

// ==========================================================
// 6. BOLHA / CARROCERIA SUPERIOR
// Conceito: sedã premium com proporções inspiradas em um Mercedes C300
// capô longo, cabine recuada, teto arqueado e traseira curta.
// ==========================================================

// Seções [x, largura, altura, z_centro]
body_sections = [
    [ -4, 100, 22, 12],   // para-choque traseiro
    [ 18, 120, 34, 18],   // tampa traseira / cola da cabine
    [ 55, 132, 50, 27],   // traseira alta
    [ 95, 130, 78, 44],   // teto traseiro
    [128, 126, 88, 50],   // topo do teto
    [156, 122, 84, 46],   // teto dianteiro
    [184, 120, 60, 34],   // base do para-brisa / capô
    [212, 112, 42, 24],   // capô avançado
    [236, 100, 36, 20],   // face frontal
    [250,  92, 30, 18]    // ponta do bico frontal
];

module section_blob(sec, shrink=0) {
    x = sec[0];
    w = max(sec[1] - 2*shrink, 2);
    h = max(sec[2] - 2*shrink, 2);
    zc = sec[3] + shrink*0.25;
    translate([x, 0, zc])
        scale([3.0, w/2, h/2]) sphere(r=1, $fn=36);
}

module sedan_outer(shrink=0) {
    intersection() {
        union() {
            for (i = [0:len(body_sections)-2])
                hull() {
                    section_blob(body_sections[i], shrink);
                    section_blob(body_sections[i+1], shrink);
                }

            // volume adicional do capô para dar frente mais "automotiva"
            hull() {
                translate([210, 0, 22]) scale([18, 52, 12]) sphere(r=1, $fn=36);
                translate([242, 0, 19]) scale([10, 45, 10]) sphere(r=1, $fn=36);
            }
        }

        // recorte para garantir fundo plano em z>=0
        translate([base_len/2, 0, 65])
            cube([base_len + 40, base_w + 40, 140], center=true);
    }
}

module front_grille_cut() {
    // grade retangular arredondada e três lâminas horizontais internas (vazadas)
    translate([245, 0, 22]) rotate([0,90,0]) {
        // janela principal da grade
        linear_extrude(height=28, center=true)
            rounded_rect_2d(40, 68, 8);
    }
}

module windshield_opening() {
    // abertura ampla para a HuskyLens, integrada como para-brisa
    hull() {
        translate([198, 0, 62]) cube([18, 64, 34], center=true);
        translate([232, 0, 76]) cube([12, 56, 30], center=true);
    }
}

module side_window_opening(side=1) {
    // abertura lateral dos vidros (efeito estético e alívio visual)
    hull() {
        translate([86, side*44, 54]) cube([22, 8, 18], center=true);
        translate([132, side*45, 68]) cube([46, 8, 24], center=true);
        translate([170, side*43, 56]) cube([22, 8, 18], center=true);
    }
}

module wheel_arch_cut(xc) {
    // recorte lateral do paralama para aliviar roda e melhorar visual.
    for (side=[-1,1])
        translate([xc, side*64, 18])
            rotate([90,0,0]) cylinder(h=26, d=72, center=true, $fn=64);
}

module bubble_shell() {
    difference() {
        union() {
            // casca principal
            difference() {
                sedan_outer(0);
                translate([0,0,bubble_wall]) sedan_outer(bubble_wall);
            }

            // flange inferior alinhada à base
            translate([base_len/2, 0, 0])
                panel_xy(base_len + 4, base_w + 4, bubble_flange_t, 12);
        }

        // abertura inferior acima da flange
        translate([base_len/2, 0, 9])
            cube([base_len - 4, base_w - 4, 18], center=true);

        // furos de fixação alinhados à base
        for (p = body_mount_pts)
            translate([p[0], p[1], bubble_flange_t/2])
                cylinder(h=20, d=bubble_mount_hole_d, center=true, $fn=24);

        // Janela da HuskyLens integrada ao para-brisa
        windshield_opening();

        // Furos do HC-SR04: embutidos na grade frontal
        for (dy = [-13, 13])
            translate([248, dy, 24]) rotate([0,90,0]) cylinder(h=34, d=19, center=true, $fn=48);

        // Fresta do IR obstáculo na tomada de ar inferior
        translate([247, 0, 12]) cube([30, 38, 9], center=true);

        // grade frontal central
        front_grille_cut();

        // rasgos horizontais da grade
        for (zv = [14, 22, 30])
            translate([246, 0, zv]) cube([26, 54, 4], center=true);

        // ventilação sobre o L298N
        for (i = [-3,-2,-1,0,1,2,3])
            translate([l298_cx, l298_cy + i*6, 74]) cube([34, 3.5, 22], center=true);

        // acesso lateral USB/barrel do Arduino Mega
        translate([mega_cx + 8, -81, 28]) cube([66, 20, 24], center=true);

        // acesso lateral powerbank
        translate([pbank_cx, -82, 24]) cube([78, 22, 26], center=true);

        // acesso chave geral
        translate([82, 82, 24]) cube([22, 20, 14], center=true);

        // vidros laterais
        side_window_opening(1);
        side_window_opening(-1);

        // recorte do vidro traseiro
        hull() {
            translate([88, 0, 58]) cube([30, 62, 22], center=true);
            translate([64, 0, 44]) cube([18, 54, 14], center=true);
        }

        // alívio das rodas / paralamas
        wheel_arch_cut(rear_axle_x);
        wheel_arch_cut(front_axle_x);
    }
}

// ==========================================================
// REFERÊNCIAS VISUAIS DO CONJUNTO
// ==========================================================

module wheel_ref(x, y) {
    translate([x, y, wheel_d/2 - base_clearance])
        rotate([90,0,0]) cylinder(h=wheel_w, d=wheel_d, center=true, $fn=64);
}

module reference_components() {
    color("lightgray") translate([pbank_cx, pbank_cy, base_t + pbank_h/2]) cube([pbank_l, pbank_w, pbank_h], center=true);
    color("royalblue") translate([mega_cx, mega_cy, base_t + 10]) cube([101.5, 53.3, 4], center=true);
    color("firebrick") translate([l298_cx, l298_cy, base_t + 16]) cube([43, 43, 27], center=true);
    color("gold") translate([pack_cx, pack_cy, base_t + pack_h/2]) cube([78, 42, pack_h], center=true);
}

module assembly() {
    // base
    color("gainsboro") base_plate();

    // tray posicionado sobre a base
    color("lightsteelblue") translate([0,0,base_t]) electronics_tray();

    // suportes
    color("silver") translate([0,0,base_t]) husky_mount();
    color("silver") translate([0,0,base_t]) hc_sr04_mount();
    color("silver") translate([0,0,base_t]) line_ir_mount();

    // bolha sobre a base
    color([0.85,0.85,0.90,0.5]) translate([0,0,base_t]) bubble_shell();

    // rodas de referência
    color("black") {
        wheel_ref(rear_axle_x,  wheel_center_y);
        wheel_ref(rear_axle_x, -wheel_center_y);
        wheel_ref(front_axle_x,  wheel_center_y);
        wheel_ref(front_axle_x, -wheel_center_y);
    }

    // componentes internos para conferência visual
    // reference_components();
}

// ==========================================================
// SELETOR DE EXPORTAÇÃO
// ==========================================================

if (part == "base")      base_plate();
if (part == "tray")      electronics_tray();
if (part == "husky")     husky_mount();
if (part == "hc_sr04")   hc_sr04_mount();
if (part == "line_ir")   line_ir_mount();
if (part == "bubble")    bubble_shell();
if (part == "assembly")  assembly();

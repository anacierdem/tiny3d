#include <libdragon.h>
#include <rspq_profile.h>

#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

#include "debug_overlay.h"
#include "joypad.h"
/*
typedef struct {
  int16_t pos[3];
  uint8_t color[3];
  int8_t size;
} __attribute__((packed)) T3DParticle;
*/
/*
typedef struct {
  int16_t pos[3];
  int16_t color;
} __attribute__((packed)) T3DParticle;
*/

typedef struct {
  int8_t posA[3];
  int8_t sizeA;
  int8_t posB[3];
  int8_t sizeB;
  int8_t colorA[3];
  int8_t texA;
  int8_t colorB[3];
  int8_t texB;
}  __attribute__((packed, aligned(__alignof__(uint32_t)))) T3DParticle;

_Static_assert(sizeof(T3DParticle) == 16, "T3DParticle size mismatch");

void generateParticles(T3DParticle *particles, int count, bool dynScale) {
  for (int i = 0; i < count; i++) {
    int p = i / 2;
    int8_t *ptPos = i % 2 == 0 ? particles[p].posA : particles[p].posB;
    int8_t *ptColor = i % 2 == 0 ? particles[p].colorA : particles[p].colorB;

    if(dynScale) {
      particles[p].sizeA = 0 + (rand() % 8);
      particles[p].sizeB = 0 + (rand() % 8);
    } else {
      particles[p].sizeA = 0;
      particles[p].sizeB = 0;
    }

    T3DVec3 pos = {{
       (i * 1 + rand()) % 64 - 32,
       (i * 3 + rand()) % 64 - 32,
       (i * 4 + rand()) % 64 - 32
     }};

    t3d_vec3_norm(&pos);
    float len = rand() % 40;
    pos.v[0] *= len;
    pos.v[1] *= len;
    pos.v[2] *= len;

    ptPos[0] = pos.v[0];
    ptPos[1] = pos.v[1];
    ptPos[2] = pos.v[2];

    ptPos[0] = (rand() % 256) - 128;
    ptPos[1] = (rand() % 256) - 128;
    ptPos[2] = (rand() % 256) - 128;

    if(dynScale) {
      ptColor[0] = 200 + (rand() % 56);
      ptColor[1] = 200 + (rand() % 56);
      ptColor[2] = 255;
    } else {
      ptColor[0] = 25 + (rand() % 230);
      ptColor[1] = 25 + (rand() % 230);
      ptColor[2] = 25 + (rand() % 230);
    }
  }
}

int main() {
  debug_init_isviewer();
  debug_init_usblog();
  asset_init_compression(2);
  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
  rdpq_init();
  rspq_profile_start();
  joypad_init();

  //rdpq_debug_start();
  //rdpq_debug_log(true);

  t3d_debug_print_init();

#define SPRITE_COUNT 7
  uint32_t spriteIdx = 0;
  sprite_t *sprites[SPRITE_COUNT] = {
    sprite_load("rom:/leafBundle00.ia8.sprite"),
    sprite_load("rom:/glass_reflection.ia8.sprite"),
    sprite_load("rom:/unit1m.i8.sprite"),
    sprite_load("rom:/fire.ia8.sprite"),
    sprite_load("rom:/leafBirch.ia8.sprite"),
    sprite_load("rom:/leafBundle03.ia8.sprite"),
    sprite_load("rom:/leafBundle05.ia8.sprite"),
  };

  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  t3d_init((T3DInitParams) {}); // Init library itself, use empty params for default settings

  T3DModel *model = t3d_model_load("rom:/sphere.t3dm");

  T3DMat4 modelMat; // matrix for our model, this is a "normal" float matrix
  t3d_mat4_identity(&modelMat);
  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  T3DMat4FP *particleMatFP = malloc_uncached(sizeof(T3DMat4FP));
  T3DMat4FP *modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  T3DVec3 camPos = {{0, 10, -18}};
  T3DVec3 camTarget = {{0, 5, 0}};

  uint8_t colorAmbient[4] = {50, 50, 50, 0xFF};
  uint8_t colorDir[4] = {0xFF, 0xFF, 0xFF, 0xFF};

  T3DVec3 lightDirVec = {{0.0f, 0.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  float rotAngle = 0.0f;
  T3DVec3 rotAxis = {{-1.0f, 2.5f, 0.25f}};
  t3d_vec3_norm(&rotAxis);

  // create a viewport, this defines the section to draw to (by default the whole screen)
  // and contains the projection & view (camera) matrices
  T3DViewport viewport = t3d_viewport_create();

  rspq_block_begin();
  t3d_model_draw(model);
  rspq_block_t *dplDraw = rspq_block_end();

  rspq_block_t *dplDrawParticle = NULL;

  float matScale = 0.1f;
  bool showMesh = false;
  bool dynScale = true;

  float posZ = 3.0f;
  float posX = 0.0f;

  //int batchSize = 256;
  int batchSize = 256;//256;
  int batchCountMax = 100;
  int batchCount = 1;
  int particleSize = 16;
  int particleCount = batchSize * batchCountMax;
  T3DParticle *particles = malloc_uncached(sizeof(T3DParticle) * particleCount / 2);
  generateParticles(particles, particleCount, dynScale);

  bool requestDisplayMetrics = false;
  bool displayMetrics = false;
  float last3dFPS = 0.0f;
  uint64_t rdpTimeBusy = 0;
  uint32_t rspTimeT3D = 0;

  for (uint64_t frame = 0;; ++frame) {
    // ======== Update ======== //
    joypad_poll();
    joypad_inputs_t inp = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_held(JOYPAD_PORT_1);
    joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(btn.c_right)batchCount++;
    if(btn.c_left)batchCount--;
    if(btn.c_up)particleSize++;
    if(btn.c_down)particleSize--;
    //if(pressed.l)spriteIdx = (spriteIdx + 1) % SPRITE_COUNT;

    if(pressed.r) {
      dynScale = !dynScale;
      generateParticles(particles, particleCount, dynScale);
    }
    if(pressed.start)showMesh = !showMesh;

    if(batchCount < 1)batchCount = 1;
    if(batchCount > batchCountMax)batchCount = batchCountMax;
    if(particleSize < 0)particleSize = 0;
    if(particleSize > 128)particleSize = 128;

    if(btn.l)rotAngle += 0.01f;
    //rotAngle += 0.01f;

    int partCountDraw = batchCount * batchSize;

    // we can set up our viewport settings beforehand here
    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 1.0f, 80.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    if(btn.a)matScale += 0.005f;
    if(btn.b)matScale -= 0.005f;
    //if(btn.a)posZ += 0.6f;
    //if(btn.b)posZ -= 0.6f;

    if(inp.stick_x < 10 && inp.stick_x > -10)inp.stick_x = 0;
    if(inp.stick_y < 10 && inp.stick_y > -10)inp.stick_y = 0;

    if(btn.z) {
      posX += inp.stick_x * -0.01f;
      posZ += inp.stick_y * 0.01f;
    } else {
      camPos.v[0] += inp.stick_x * -0.01f;
      camPos.v[2] += inp.stick_y * 0.01f;
      camTarget.v[0] += inp.stick_x * -0.01f;
      camTarget.v[2] += inp.stick_y * 0.01f;
    }

    if(btn.d_down)
    {
      requestDisplayMetrics = true;
    } else {
      requestDisplayMetrics = false;
      displayMetrics = false;
    }

    // Model-Matrix, t3d offers some basic matrix functions
    t3d_mat4fp_from_srt_euler(
      particleMatFP,
      (float[3]){matScale, matScale, matScale},
      (float[3]){rotAngle * 0.1f, rotAngle, rotAngle * 0.3f},
      (float[3]){posX, 10.0f, posZ}
    );
    t3d_mat4fp_from_srt_euler(
      modelMatFP,
      (float[3]){0.125f, 0.125f, 0.125f},
      (float[3]){0,0,0},
      (float[3]){0, 0, 0}
    );

    // ======== Draw (3D) ======== //
    surface_t *display = display_get();
    rdpq_attach(display, display_get_zbuf()); // set the target to draw to
    t3d_frame_start(); // call this once per frame at the beginning of your draw function

    t3d_viewport_attach(&viewport); // now use the viewport, this applies proj/view matrices and sets scissoring

    //rdpq_mode_combiner(RDPQ_COMBINER_FLAT);

    // this cleans the entire screen (even if out viewport is smaller)
    t3d_screen_clear_color(RGBA32(10, 10, 10, 0));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorDir); // one global ambient light, always active
    //t3d_light_set_directional(0, colorDir, &lightDirVec); // optional directional light, can be disabled
    t3d_light_set_count(0);

    rdpq_sync_pipe();

    //if(!showMesh && particleSize > 0) {
    if(particleSize > 0) {
      rdpq_mode_push();
        //rdpq_mode_combiner(RDPQ_COMBINER1((TEX0,0,PRIM,0),    (0,0,0,TEX0)));
        rdpq_mode_combiner(RDPQ_COMBINER1((0,0,0,PRIM),    (0,0,0,1)));
        rdpq_mode_antialias(AA_NONE);
        rdpq_mode_dithering(DITHER_NONE_NONE);
        rdpq_mode_zoverride(true, 0.5f, 0);
        rdpq_mode_zbuf(true, true);
        rdpq_mode_persp(false);
        rdpq_mode_alphacompare(128);

        //rdpq_sprite_upload(TILE0, sprites[spriteIdx], NULL);

      t3d_state_set_drawflags(T3D_FLAG_DEPTH);
      t3d_matrix_push(particleMatFP);

      t3d_pipeline_load(T3D_PIPELINE_PARTICLES);

      uint32_t addr = PhysicalAddr(particles);
      for(int i=0; i<partCountDraw; i+=batchSize) {
        int16_t imgSize = (sprites[spriteIdx]->width << 11) / particleSize;
        //t3d_particles_draw((void*)addr, sizeof(T3DParticle)*batchSize, particleSize, imgSize);
        //t3d_particles_draw((void*)addr, sizeof(T3DParticle)*batchSize, particleSize, PhysicalAddr(display->buffer));

        uint32_t dataSize = sizeof(T3DParticle) * batchSize/2;
        dataSize &= 0xFFFF;

        t3d_state_set_vertex_fx(T3D_VERTEX_FX_NONE, particleSize, 0);
        rspq_write(T3D_RSP_ID, T3D_CMD_VERT_LOAD,
          dataSize, addr,
          0
        );

        addr += sizeof(T3DParticle) * batchSize/2;
      }

      t3d_pipeline_load(T3D_PIPELINE_DEFAULT);
      t3d_matrix_pop(1);

      rdpq_sync_pipe();
      rdpq_mode_pop();
    }

    if(showMesh)
    {
      t3d_matrix_push(modelMatFP);
      rspq_block_run(dplDraw);
      t3d_matrix_pop(1);
    }

    rdpq_sync_pipe();

    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 16, "FPS: %.2f", display_get_fps());
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 30, "Point: %d @ %d", partCountDraw, particleSize);

    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 240-30, "RSP/t3d: %ldus", rspTimeT3D);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 240-18, "RDP    : %lldus", rdpTimeBusy);



    if(displayMetrics)
    {
      t3d_debug_print_start();
      if(profile_data.frame_count == 0) {
        t3d_debug_printf(140, 206, "FPS (3D)   : %.4f", last3dFPS);
        t3d_debug_printf(140, 218, "FPS (3D+UI): %.4f", display_get_fps());
      }
      debug_draw_perf_overlay(last3dFPS);
      rspq_wait();
    }

    rdpq_detach_show();
    rspq_profile_next_frame();

    if(frame == 30)
    {
      if(!displayMetrics){
        last3dFPS = display_get_fps();
        rspq_wait();
        rspq_profile_get_data(&profile_data);

        rdpTimeBusy = RCP_TICKS_TO_USECS(profile_data.rdp_busy_ticks / profile_data.frame_count);
        rspTimeT3D = RCP_TICKS_TO_USECS(profile_data.slots[2].total_ticks / profile_data.frame_count);

        if(requestDisplayMetrics)displayMetrics = true;
      }

      frame = 0;
      rspq_profile_reset();
    }
  }

  t3d_destroy();
  return 0;
}


#include <libdragon.h>
#include <rspq_profile.h>

#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

#include "debug_overlay.h"
#include "joypad.h"

void generateParticles(T3DParticlePacked *particles, int count, bool dynScale) {
  for (int i = 0; i < count; i++) {
    int p = i / 2;
    int8_t *ptPos = i % 2 == 0 ? particles[p].posA : particles[p].posB;
    uint8_t *ptColor = i % 2 == 0 ? particles[p].colorA : particles[p].colorB;

    if(dynScale) {
      particles[p].sizeA = 0 + (rand() % 64);
      particles[p].sizeB = 0 + (rand() % 64);
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

//    ptPos[0] = pos.v[0];
//    ptPos[1] = pos.v[1];
//    ptPos[2] = pos.v[2];

    ptPos[0] = (rand() % 256) - 128;
    ptPos[1] = (rand() % 256) - 128;
    ptPos[2] = (rand() % 256) - 128;

    ptColor[3] = rand() % 8;

    if(dynScale) {
      // random red/orange fire color

      /*ptColor[0] = (uint8_t)((int)ptPos[0] + 128);
      ptColor[1] = (uint8_t)((int)ptPos[1] + 128);
      ptColor[2] = (uint8_t)((int)ptPos[2] + 128);*/

      ptColor[0] = 200 + (rand() % 56);
      ptColor[1] = 100 + (rand() % 156);
      ptColor[2] = 25 + (rand() % 230);

    } else {
      ptColor[0] = 25 + (rand() % 230);
      ptColor[1] = 25 + (rand() % 230);
      ptColor[2] = 25 + (rand() % 230);
    }
  }
}
void gradient_fire(uint8_t *color, float t) {
    //t = (1.0f - t);
    t = fminf(1.0f, fmaxf(0.0f, t));

    if (t < 0.0f) {
        t = 0.0f;
    } else if (t > 1.0f) {
        t = 1.0f;
    }

    if (t < 0.25f) {
        // Dark red to bright red
        color[0] = (uint8_t)(200 * (t / 0.25f)) + 55;
        color[1] = 0;
        color[2] = 0;
    } else if (t < 0.5f) {
        // Bright red to yellow
        color[0] = 255;
        color[1] = (uint8_t)(255 * ((t - 0.25f) / 0.25f));
        color[2] = 0;
    } else if (t < 0.75f) {
        // Yellow to white (optional, if you want a bright white center)
        color[0] = 255;
        color[1] = 255;
        color[2] = (uint8_t)(255 * ((t - 0.5f) / 0.25f));
    } else {
        // White to black
        color[0] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
        color[1] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
        color[2] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
    }
}

static int currentPart  = 0;
void simulate_particles(T3DParticlePacked *particles, int partCount, float posX, float posZ) {
  int p = currentPart / 2;
  if(currentPart % (1+(rand() % 4)) == 0) {
    int8_t *ptPos = currentPart % 2 == 0 ? particles[p].posA : particles[p].posB;
    int8_t *size = currentPart % 2 == 0 ? &particles[p].sizeA : &particles[p].sizeB;
    ptPos[0] = posX + (rand() % 16) - 8;
    ptPos[1] = -127;
    ptPos[2] = posZ + (rand() % 16) - 8;
    *size = 60 + (rand() % 10);
  }
  currentPart = (currentPart + 1) % partCount;

  // move all up by one unit
  for (int i = 0; i < partCount/2; i++) {
    gradient_fire(particles[i].colorA, (particles[i].posA[1] + 127) / 150.0f);
    gradient_fire(particles[i].colorB, (particles[i].posB[1] + 127) / 150.0f);

    particles[i].posA[1] += 1;
    particles[i].posB[1] += 1;
    if(currentPart % 4 == 0) {
      particles[i].sizeA -= 2;
      particles[i].sizeB -= 2;
      if(particles[i].sizeA < 0)particles[i].sizeA = 0;
      if(particles[i].sizeB < 0)particles[i].sizeB = 0;
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

  t3d_debug_print_init();

#define SPRITE_COUNT 7
  uint32_t spriteIdx = 0;
  sprite_t *sprites[SPRITE_COUNT] = {
    sprite_load("rom:/anim.ia8.sprite"),
    sprite_load("rom:/unit1m.i8.sprite"),
    sprite_load("rom:/leafBundle00.ia8.sprite"),
    sprite_load("rom:/glass_reflection.ia8.sprite"),
    sprite_load("rom:/fire.ia8.sprite"),
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
  uint8_t colorDir[4] = {0xbF, 0xbF, 0xbF, 0xFF};

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
  matScale = 0.04f;
  bool showMesh = false;
  bool dynScale = true;
  bool useTex = true;

  float posZ = 3.0f;
  float posX = 0.0f;

  //int batchSize = 256;
  int batchSize = 64;
  int batchCountMax = 100;
  int batchCount = 1;
  float particleSize = 55;
  int particleCount = batchSize * batchCountMax;
  T3DParticlePacked *particles = malloc_uncached(sizeof(T3DParticlePacked) * particleCount / 2);
  generateParticles(particles, particleCount, dynScale);

  bool requestDisplayMetrics = false;
  bool displayMetrics = false;
  float last3dFPS = 0.0f;
  uint64_t rdpTimeBusy = 0;
  uint32_t rspTimeT3D = 0;
  uint8_t uvOffset = 0;

  for (uint64_t frame = 0;; ++frame) {
    // ======== Update ======== //
    joypad_poll();
    joypad_inputs_t inp = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_held(JOYPAD_PORT_1);
    joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(btn.c_right)batchCount++;
    if(btn.c_left)batchCount--;
    if(btn.c_up)particleSize+=0.25f;
    if(btn.c_down)particleSize-=0.25f;
    if(pressed.l)spriteIdx = (spriteIdx + 1) % SPRITE_COUNT;

    if(pressed.r) {
      dynScale = !dynScale;
      generateParticles(particles, particleCount, dynScale);
    }
    if(pressed.start)showMesh = !showMesh;
    if(pressed.d_up)useTex = !useTex;

    if(batchCount < 1)batchCount = 1;
    if(batchCount > batchCountMax)batchCount = batchCountMax;
    if(particleSize < 0)particleSize = -0.01f;
    if(particleSize > 128)particleSize -= 128;

    //if(btn.l)rotAngle += 0.01f;
    //rotAngle += 0.01f;

    if(frame % 15 == 0)uvOffset = (uvOffset + 1) % 8;
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
      posX += inp.stick_x * -0.03f;
      posZ += inp.stick_y * 0.03f;
    } else {
      camPos.v[0] += inp.stick_x * -0.001f;
      camPos.v[2] += inp.stick_y * 0.001f;
      camTarget.v[0] += inp.stick_x * -0.001f;
      camTarget.v[2] += inp.stick_y * 0.001f;
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
      (float[3]){matScale * 2.0f, matScale, matScale * 2.0f},
      (float[3]){rotAngle * 0.1f, rotAngle, rotAngle * 0.3f},
      //(float[3]){posX, 10.0f, posZ}
      (float[3]){0, 160.0f * matScale, 0}
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
    if(particleSize >= 0) {
      rdpq_mode_push();
        if(useTex) {
          rdpq_mode_combiner(RDPQ_COMBINER1((TEX0,0,PRIM,0),    (0,0,0,TEX0)));
        } else {
          rdpq_mode_combiner(RDPQ_COMBINER1((0,0,0,PRIM),    (0,0,0,1)));
        }

        rdpq_mode_antialias(AA_NONE);
        rdpq_mode_dithering(DITHER_NONE_NONE);
        rdpq_mode_zoverride(true, 0.5f, 0);
        rdpq_mode_zbuf(true, true);
        rdpq_mode_persp(false);
        rdpq_mode_alphacompare(128);

        if(useTex) {
          rdpq_sprite_upload(TILE0, sprites[spriteIdx], NULL);
          t3d_state_set_drawflags(T3D_FLAG_DEPTH | T3D_FLAG_TEXTURED);
        } else {
          t3d_state_set_drawflags(T3D_FLAG_DEPTH);
        }


      t3d_matrix_push(particleMatFP);

      t3d_pipeline_load(T3D_PIPELINE_PARTICLES);

      //rdpq_debug_log(true);

      uint32_t addr = PhysicalAddr(particles);
      for(int i=0; i<partCountDraw; i+=batchSize) {
        int particleSizeMax = 64;
        t3d_particles_set_params(particleSize, particleSizeMax*2-1, particleSizeMax,
          uvOffset
        );

        uint32_t textureSize = sprites[spriteIdx]->height;
        //t3d_particles_set_size(0, 0xFF, 0);

        if(useTex) {
          rspq_write(T3D_RSP_ID, T3D_CMD_VERT_LOAD,
             sizeof(T3DParticlePacked) * batchSize/2, addr,
             ((0xFF) << 16) | (textureSize / 16)
          );
        } else {
          t3d_particles_draw_color((T3DParticlePacked*)(void*)addr, batchSize);
        }

        addr += sizeof(T3DParticlePacked) * batchSize / 2;
      }

      //rdpq_debug_log(false);

      //simulate_particles(particles, partCountDraw, posX, posZ);

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
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 30, "Point: %d @ %.2f", partCountDraw, particleSize);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 44, "UV: %d", uvOffset);

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


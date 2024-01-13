#include "STB_Text.h"
#include "Application.h"
#include "ModuleEditor.h"

STB_Text::STB_Text()
{
    fontPath = "c:/windows/fonts/times.ttf";
}

STB_Text::~STB_Text()
{
}

void STB_Text::Init()
{
    fread(ttf_buffer, 1, 1 << 20, fopen(fontPath.c_str(), "rb"));
    stbtt_BakeFontBitmap(ttf_buffer, 0, 64.0, temp_bitmap, 1024, 1024, 32, 96, cdata);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 1024, 1024, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void STB_Text::Print(float x, float y, const char* text)
{
    // assume orthographic projection with units = screen pixels, origin at top left
    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_QUADS);
    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, 1024, 1024, *text - 32, &x, &y, &q, 1);//1=opengl & d3d10+,0=d3d9
            glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y0);
            glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y0);
            glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y1);
            glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y1);
        }
        ++text;
    }
    glEnd();
}

void STB_Text::Draw(float x, float y, std::string& text)
{
    glViewport(0, 0, App->editor->gameViewSize.x, App->editor->gameViewSize.y);
    glClearColor(0.45f, 0.45f, 0.75f, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, App->editor->gameViewSize.x, App->editor->gameViewSize.y, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1, 1, 1);

    Print(100, 150, text.c_str());

    // show font bitmap
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(256, 200 + 0);
    glTexCoord2f(1, 0); glVertex2f(768, 200 + 0);
    glTexCoord2f(1, 1); glVertex2f(768, 200 + 512);
    glTexCoord2f(0, 1); glVertex2f(256, 200 + 512);
    glEnd();
}

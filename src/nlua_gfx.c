/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file nlua_gfx.c
 *
 * @brief Handles the rendering of graphics on the screen.
 */

#include "nlua_gfx.h"

#include "naev.h"

#include "lauxlib.h"

#include "nlua.h"
#include "nluadef.h"
#include "log.h"
#include "opengl.h"
#include "nlua_tex.h"


/* Ship metatable methods. */
static int gfxL_dim( lua_State *L );
static int gfxL_renderTex( lua_State *L );
static int gfxL_renderRect( lua_State *L );
static const luaL_reg gfxL_methods[] = {
   { "dim", gfxL_dim },
   { "renderTex", gfxL_renderTex },
   { "renderRect", gfxL_renderRect },
   {0,0}
}; /**< Ship metatable methods. */




/**
 * @brief Loads the graphics library.
 *
 *    @param L State to load graphics library into.
 *    @return 0 on success.
 */
int nlua_loadGFX( lua_State *L, int readonly )
{
   if (readonly) /* Nothing is read only */
      return 0;

   /* Register the values */
   luaL_register(L, "gfx", gfxL_methods);

   return 0;
}


/**
 * @brief Lua bindings to interact with rendering and the NAEV graphical environment.
 *
 * An example would be:
 * @code
 * t  = tex.open( "gfx/foo/bar.png" ) -- Loads the texture
 * gfx.renderTex( t, 0., 0. ) -- Draws texture at origin
 * @endcode
 *
 * @luamod gfx
 */


/**
 * @brief Gets the dimensions of the NAEV window.
 *
 * @usage screen_w, screen_h = gfx.dim()
 *
 *    @luareturn The width and height of the NAEV window.
 * @luafunc dim()
 */
static int gfxL_dim( lua_State *L )
{
   lua_pushnumber( L, SCREEN_W );
   lua_pushnumber( L, SCREEN_H );
   return 2;
}


/**
 * @brief Renders a texture.
 *
 * This function has variable parameters depending on how you want to render.
 *
 * @usage gfx.renderTex( tex, 0., 0. ) -- Render tex at origin
 * @usage gfx.renderTex( tex, 0., 0., col ) -- Render tex at origin with colour col
 * @usage gfx.renderTex( tex, 0., 0., 4, 3 ) -- Render sprite at position 4,3 (top-left is 1,1)
 * @usage gfx.renderTex( tex, 0., 0., 4, 3, col ) -- Render sprite at position 4,3 (top-left is 1,1) with colour col
 *
 *    @luaparam tex Texture to render.
 *    @luaparam pos_x X position to render texture at.
 *    @luaparam pos_y Y position to render texture at.
 *    @luaparam sprite_x X sprite to render.
 *    @luaparam sprite_y Y sprite to render.
 *    @luaparam colour Colour to use when rendering.
 * @luafunc renderTex( tex, pos_x, pos_y, sprite_x, sprite_y, colour )
 */
static int gfxL_renderTex( lua_State *L )
{
   LuaTex *lt;
   double x, y;
   int sx, sy;

   /* Parameters. */
   lt = luaL_checktex( L, 1 );
   x  = luaL_checknumber( L, 2 );
   y  = luaL_checknumber( L, 3 );
   if (lua_isnumber( L, 4 )) {
      sx    = luaL_checkinteger( L, 4 ) - 1;
      sy    = luaL_checkinteger( L, 5 ) - 1;
   }
   else {
      sx    = 0;
      sy    = 0;
   }

   /* Some sanity checking. */
   if (sx >= lt->tex->sx)
      NLUA_ERROR( L, "Texture '%s' trying to render out of bounds (X position) sprite: %d > %d.",
            lt->tex->name, sx+1, lt->tex->sx );
   if (sx >= lt->tex->sx)
      NLUA_ERROR( L, "Texture '%s' trying to render out of bounds (Y position) sprite: %d > %d.",
            lt->tex->name, sy+1, lt->tex->sy );

   /* Render. */
   gl_blitStaticSprite( lt->tex, x, y, sx, sy, NULL );

   return 0;
}


/**
 * @brief Renders a rectangle.
 *
 * @usage gfx.renderRect( 10., 30,. 40., 40., col ) -- Renders a 40 side square at position 10,30 of colour col
 * @usage gfx.renderRect( 10., 30,. 40., 40., col, True ) -- Renders a 40 side empty square at position 10,30 of colour col
 *
 *    @luaparam x X position to render at.
 *    @luaparam y Y position to render at.
 *    @luaparam w Width of the rectangle.
 *    @luaparam h Height of the rectangle.
 */
static int gfxL_renderRect( lua_State *L )
{
   double x,y, w,h;
   int empty;

   /* Parse parameters. */
   x     = luaL_checknumber( L, 1 ) - SCREEN_W/2.;
   y     = luaL_checknumber( L, 2 ) - SCREEN_H/2.;
   w     = luaL_checknumber( L, 3 );
   h     = luaL_checknumber( L, 4 );
   empty = lua_toboolean( L, 6 );

   /* Render. */
   if (empty)
      gl_renderRectEmpty( x, y, w, h, &cWhite );
   else
      gl_renderRect( x, y, w, h, &cWhite );

   return 0;
}




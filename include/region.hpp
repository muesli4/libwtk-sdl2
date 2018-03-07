#ifndef LIBWTK_SDL2_REGION_HPP
#define LIBWTK_SDL2_REGION_HPP

#include <SDL2/SDL_types.h>

#include "geometry.hpp"

struct size_hint
{
    size_hint(vec min, vec nat);
    size_hint(vec min);

    vec minimal;
    vec natural;
};

/**
 * A region whose space gets allocated with a specific size.
 */
struct region
{
    /**
     * @name Layout
     * @{
     */

    /**
     * In the simplest case this just sets the bounding box but may involve more
     * complex calculations (e.g., with widget containers).
     */
    void apply_layout(SDL_Rect box);

    /**
     * Refresh layout with existing box. This should not be called if the region
     * does not have a box already. Otherwise no space is allocated.
     */
    virtual void on_box_allocated();

    // TODO change encoding: none(), height(int), width(int)
    /**
     * Computes the region's preferred minimal and natural size. When given a
     * specific width or height the region might use that to give a more
     * accurate size.
     */
    virtual size_hint get_size_hint(int width = -1, int height = -1) const = 0;

    /**
     * Whether a region is able to use an intermediate value between minimal and
     * natural size (e.g., assigning more width or height). This might change
     * the aspect ratio of the region.
     *
     * Whenever a region does not fit with natural but more than minimal, some
     * regions profit from using additional space. For example, an image widget
     * in aspect ratio or a list view. A label in general can't profit from
     * extra space and would only look awkward.
     */
    virtual bool can_use_intermediate_size() const;

    /** @} */


    /**
     * Get the region's assigned bounding box.
     */
    SDL_Rect const & get_box() const;

    private:

    SDL_Rect _box;
};

#endif


#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/widgets/scroll.hpp>

#include <vector>
#include <memory>

#include <fmt/format.h>

template<bool vertical = true>
class gallery : public nana::panel<true>, public std::vector<std::shared_ptr<nana::picture>>
{
    enum offset
    {
        left,
        right,
        both//равномерно справа и слева
    };

    nana::size _image_size = {100, 100};
    size_t _gap = 4;
    offset _offset_side = offset::right;
    double _rows_per_wheel = 0.5;
public:
    gallery()
        : nana::panel<true>() { init(); }

    gallery(nana::window wd)
        : nana::panel<true>(wd) { init(); }
public:
    void image_size(const nana::size& image_size)
    {
        _image_size = image_size;
        scrollbar.step(wheel_amount());
    }
    void image_size(size_t image_width, size_t image_height)
    {
        _image_size.width = image_width;
        _image_size.height = image_height;
        scrollbar.step(wheel_amount());
    }
    nana::size& image_size()
    {
        return _image_size;
    }

    void gap(size_t gap)
    {
        _gap = gap + gap % (int)(1 / 0.5);
    }
    size_t gap()
    {
        return _gap;
    }

    void offset_side(offset offset_size)
    {
        _offset_side = offset_size;
    }
    offset offset_side()
    {
        return _offset_side;
    }

    size_t row_height()
    {
        return _image_size.height + _gap;
    }

    size_t wheel_amount()
    {
        return (double)row_height() * _rows_per_wheel;
    }

    double rows_per_wheel()
    {
        return _rows_per_wheel;
    }
    void rows_per_wheel(double rows_per_wheel)
    {
        _rows_per_wheel = rows_per_wheel;
    }

    using base = nana::panel<true>;
    using pictures = std::vector<std::shared_ptr<nana::picture>>;
    using pictures::size;
public:

    size_t count()
    {
        return pictures::size();
    }


    void resize()
    {
        auto arg = base::size();
        if constexpr (vertical)
            onresize(arg.width, arg.height, _image_size.width, _image_size.height, "");
        else
            onresize(arg.height, arg.width, _image_size.height, _image_size.width, "vert");
    }


    void invalidate()
    {
//        scrollable_panel_place["gallery"].fasten(scrollable_panel);
//        for(auto& pic : *this)
//            scrollable_panel_place.erase(*pic);
        for(auto& pic : *this)
        {
            if (pic->parent() != scrollable_panel)
                nana::API::set_parent_window(*pic, scrollable_panel);
            scrollable_panel_place.erase(*pic);
            scrollable_panel_place["gallery"] << *pic;
        }

        resize();


//        scrollable_panel_place.collocate();
    }

    void show(const nana::picture& pic)
    {
        if (pic.parent() != scrollable_panel)
            nana::API::set_parent_window(pic, scrollable_panel);
        scrollable_panel_place["gallery"] << pic;
        resize();
    }

    struct size
    {
        size_t rows;
        size_t cols;
    };

    struct size dimensions()
    {
        auto count = gallery::count();
        auto cols = gallery::cols();
        auto rows = count / cols + (size_t)(bool)(count % cols);
        return {rows, cols};
    }

    size_t cols()
    {
        auto width = scrollable_panel.size().width;
        auto img_width = _image_size.width;
        auto gap = _gap;
        auto cols = (width + gap) / (img_width + gap);
        return cols;
    }

    size_t rows()
    {
        return dimensions().rows;
    }

    size_t displaying_rows()
    {
        auto row_height = gallery::row_height();
        auto displaying_rows = base::size().height / row_height + 1;
        return displaying_rows;
    }

    size_t displaying_cols()
    {
        return dimensions().cols;
    }

    pictures displayable_pictures()
    {
        auto scrolled_height = -scrollable_panel.pos().y;
        auto row_height = gallery::row_height();
        auto scrolled_rows = scrolled_height / row_height;
        auto displaying_rows = base::size().height / row_height + 1;
        auto cols = gallery::cols();

        fmt::print("scrolled_rows = {}\ndisplaying_rows = {}\n", scrolled_rows, displaying_rows);

        return {
            std::next(std::begin(*this), scrolled_rows * cols),
            std::min(std::next(std::begin(*this), (scrolled_rows + displaying_rows + 1) * cols), std::end(*this))
        };
    }


public:
    struct arg_scroll : public nana::event_arg
    {
        int position;
        arg_scroll(int position) : position(position) {}
    };

    nana::basic_event<arg_scroll> scroll;

    struct Events
    {
        virtual ~Events() = default;
        nana::basic_event<nana::arg_mouse>& mouse_enter;
        nana::basic_event<nana::arg_mouse>& mouse_move;
        nana::basic_event<nana::arg_mouse>& mouse_leave;
        nana::basic_event<nana::arg_mouse>& mouse_down;
        nana::basic_event<nana::arg_mouse>& mouse_up;
        nana::basic_event<nana::arg_click>& click;
        nana::basic_event<nana::arg_mouse>& dbl_click;
        nana::basic_event<nana::arg_wheel>& mouse_wheel;
        nana::basic_event<nana::arg_dropfiles>&	mouse_dropfiles;
        nana::basic_event<nana::arg_expose>&	expose;
        nana::basic_event<nana::arg_focus>&	focus;
        nana::basic_event<nana::arg_keyboard>&	key_press;
        nana::basic_event<nana::arg_keyboard>&	key_release;
        nana::basic_event<nana::arg_keyboard>&	key_char;
        nana::basic_event<nana::arg_keyboard>&	shortkey;
        nana::basic_event<nana::arg_move>&		move;
        nana::basic_event<nana::arg_resizing>&	resizing;
        nana::basic_event<nana::arg_resized>&	resized;
        nana::basic_event<nana::arg_destroy>&	destroy;

        nana::basic_event<arg_scroll>&	scroll;


        Events(event_type& events, nana::basic_event<arg_scroll>& scroll) :
            mouse_enter(events.mouse_enter),
            mouse_move(events.mouse_move),
            mouse_leave(events.mouse_leave),
            mouse_down(events.mouse_down),
            mouse_up(events.mouse_up),
            click(events.click),
            dbl_click(events.dbl_click),
            mouse_wheel(events.mouse_wheel),
            mouse_dropfiles(events.mouse_dropfiles),
            expose(events.expose),
            focus(events.focus),
            key_press(events.key_press),
            key_release(events.key_release),
            key_char(events.key_char),
            shortkey(events.shortkey),
            move(events.move),
            resizing(events.resizing),
            resized(events.resized),
            destroy(events.destroy),
            scroll(scroll)
        {}
    };

    Events events()
    {
        return { base::events(), scroll };
    }

private:
    nana::panel<false> scrollable_panel{ *this };
    nana::place scrollable_panel_place{ scrollable_panel };
    nana::scroll<vertical> scrollbar{ *this, {} };
    nana::place place{ *this };

    void init()
    {
        std::string div = "<scrollable_panel><weight=16 scroll>";
        if constexpr (!vertical)
            div = "vert" + div;
        place.div(div);
        place["scroll"] << scrollbar;
        place.collocate();

        scrollbar.events().value_changed([&]
        {
            scrollable_panel.move(0, -(int)scrollbar.value());
        });

        base::events().mouse_wheel([&](nana::arg_wheel arg){
            scrollbar.make_page_scroll(!arg.upwards);
        });

        base::events().resized([&](nana::arg_resized arg){
            if constexpr (vertical)
                onresize(arg.width, arg.height, _image_size.width, _image_size.height, "");
            else
                onresize(arg.height, arg.width, _image_size.height, _image_size.width, "vert");
        });

        scrollable_panel.events().move([&](nana::arg_move arg){
           scroll.emit(arg_scroll(-arg.y), *this);
        });


        scrollable_panel.events().key_press([&](nana::arg_keyboard arg){
            bool forward;
            switch(arg.key)
            {
            case 33:forward = false; break;//PageUp
            case 34:forward = true; break;//PageDown
            default: return;
            }
            fmt::print("{}\n", displaying_rows());
            scrollbar.make_step(forward, (1. / _rows_per_wheel) * displaying_rows());
        });
    }

    void onresize(size_t width, size_t height, size_t img_width, size_t img_height, std::string div){
        auto& gap = _gap;
        size_t x, offset, rows, cols, count = pictures::size();

        //(img_width + gap) * x - gap = width
        x = (width + gap) / (img_width + gap);
        offset = (width + gap) % (img_width + gap);

        cols = x;
        rows = count / x + (size_t)(bool)(count % x);
        auto scrollable_panel_height = (img_height + gap) * rows - gap;

        std::string scrollable_panel_div = fmt::format("<gallery gap={gap} grid=[{cols}, {rows}]>", fmt::arg("rows", rows), fmt::arg("cols", cols), fmt::arg("gap", gap));

        const char* str = offset_layouts[_offset_side];
        //div += fmt::format(offset_layouts[_offset_side], fmt::arg("gallery", scrollable_panel_div), fmt::arg("offset", offset) );
        div += fmt::vformat(offset_layouts[_offset_side], fmt::make_format_args(fmt::arg("gallery", scrollable_panel_div), fmt::arg("offset", offset)));

        scrollable_panel_place.div(div);
//            scrollable_panel_place.collocate();

        scrollable_panel.size(nana::size(width, scrollable_panel_height));
        scrollbar.amount(scrollable_panel_height - height);
    }

    static constexpr const char* offset_layouts[3] {
        "<weight={offset}><{gallery}>",
        "<{gallery}><weight={offset}>",
        "<weight={offset}><{gallery}><weight={offset}>"
    };
};


#include <nana/gui.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/place.hpp>

#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/filebox.hpp>

#include <iostream>
#include <type_traits>
#include <nana/threads/pool.hpp>
#include <filesystem>
namespace fs = std::filesystem;

#include <gallery.h>
#include <picturebox.h>

int main()
{
    using namespace nana;

    form fm{ API::make_center(400 * 1.618, 400) };
    fm.div("vert<<addfolder fit><subdirectories margin=[5, 0, 0] fit><><clear fit> weight=30><gallery><<info fit><>weight=20>");


    gallery gallery(fm);
    gallery.image_size(180, 180);
    gallery.bgcolor(color("#f6f6f6"));
    fm["gallery"] << gallery;


    button addfolder(fm, "addfolder");
    button clear(fm, "clear");
    checkbox subdirectories(fm, "subdirectories");
    checkbox load(fm, "load"); load.check(true);
    label all(fm), added(fm);
    fm["addfolder"] << addfolder;
    fm["clear"] << clear;
    fm["subdirectories"] << subdirectories << load;
    fm["info"] << all << added;
    fm.collocate();
    constexpr const char* added_fmt = "(+{})";


    nana::threads::pool pool;
    std::mutex mutex;

    decltype(gallery.displayable_pictures()) displayable_pictures;
    gallery.events().scroll([&](decltype(gallery)::arg_scroll arg){

        if (!load.checked()) return;

        pool.push([&]{

            mutex.lock();

            auto& old_orders = displayable_pictures;
            auto  new_orders = gallery.displayable_pictures();
            decltype(new_orders) cut_orders;


            std::set_difference(old_orders.begin(), old_orders.end(),
                                  new_orders.begin(), new_orders.end(),
                                  std::back_inserter(cut_orders),
                                  [](auto& a, auto& b) { return a->handle() < b->handle(); });

            for(auto& val : new_orders)
            {
                auto& pic = (picturebox&)*val;
                pic.load();
            }

//            for(auto& val : cut_orders)
//            {
//                auto& pic = (picturebox&)*val;
//                pic.unload();
//            }

            displayable_pictures = new_orders;

            mutex.unlock();
        });
    });


    folderbox box{ fm };
    addfolder.events().click([&](arg_click arg){
        std::thread th([&]{
            auto res = box.show();
            if (!res.empty())
            {
                int i = 0;
                added.caption(fmt::format(added_fmt, i));
                auto path = res[0];

                auto glambda = [&]<class T>(T a) {
                    for(const fs::directory_entry& p : a)
                    {
                        auto res = new picturebox(gallery);
                        res->source(p.path());
//                        res->load();
//                        gallery.show(**gallery.emplace(gallery.cbegin(), res));
                        gallery.emplace(gallery.cbegin(), res);

                        all.caption(std::to_string(gallery.size()));
                        added.caption(fmt::format(added_fmt, ++i));
                        fm.collocate();
                    }
                };
                (subdirectories.checked() ? glambda(fs::recursive_directory_iterator(path)) : glambda(fs::directory_iterator(path)));
                gallery.invalidate();
            }
        });
        th.detach();
    });
    clear.events().click([&](arg_click arg){
        gallery.clear();
    });


    fm.show();
    exec();
}

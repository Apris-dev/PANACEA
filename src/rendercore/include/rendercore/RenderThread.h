#pragma once

#include <functional>

#include "Renderer.h"
#include "sptr/Memory.h"
#include "sstl/Vector.h"
#include "sutil/Threading.h"

#include "tracy/Tracy.hpp"
#include "tracy/TracyC.h"

class CRenderThread : public CPersistentThread {

public:

    struct Context : SRendererInfo {

        template <typename TRendererType>
        requires std::is_base_of_v<CObjectRenderer, TRendererType>
        void addRendererProxy(const TRendererType::Proxy& inProxy) {

        }
    };

    //TODO: fill ctx
    CRenderThread(): CPersistentThread(m_Worker) {
        m_Worker.add([&] {
            TracyCSetThreadName("Render Thread")
            Context ctx;
            while (true) {
                TracyCZoneNC(zone, "Render Thread", 0xff0000, 1);
                {
                    ZoneScopedN("Render");
                    ctx.renderer->render(ctx);
                }
                {
                    ZoneScopedN("Commands");
                    while (!mRendererTaskQueue.isEmpty()) {
                        mRendererTaskQueue.top()(ctx);
                        mRendererTaskQueue.pop();
                    }
                }
                TracyCZoneEnd(zone);
            }
        });
    }

    void enqueue(const std::function<void(Context&)>& func) {
        mRendererTaskQueue.push(func);
    }

private:

    CWorker m_Worker;

    TVector<std::function<void(Context&)>> mRendererTaskQueue;
};

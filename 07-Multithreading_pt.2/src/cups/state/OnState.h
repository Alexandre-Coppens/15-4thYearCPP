#pragma once

#include "PrinterState.h"
#include "IsPrintingState.h"
#include "OffState.h"

#include "../Printer.h"
#include "../Job.h"

namespace cups
{
    class OnState : public PrinterState
    {
        public:
            OnState(Printer *printer) noexcept;
            
            bool stop() const override;
            bool print(Job *job) const override;
            
            bool isAvailable() const override;
        protected:
            Job *job;
    };
}

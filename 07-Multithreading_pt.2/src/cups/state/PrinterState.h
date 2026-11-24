#pragma once

#include "IPrinterState.h"

namespace cups
{
    class Printer;
    
    class PrinterState : public IPrinterState
    {
        public:
            PrinterState(Printer *printer) noexcept
                : printer(printer)
            {
            }
            
            bool start() const override
            {
                return error();
            }

            bool stop() const override
            {
                return error();
            }

            bool print(Job *job) const override
            {
                return error();
            }

            bool printingCompleted() const override
            {
                return error();
            }

            bool suspend() const override
            {
                return error();
            }

            bool resume() const override
            {
                return error();
            }

            bool cancel() const override
            {
                return error();
            }
        protected:
            static bool error()
            {
                throw "Erreur";
                return false;
            }
        protected:
            Printer *printer;
    };
}

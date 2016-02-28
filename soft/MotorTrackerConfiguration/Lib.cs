using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Globalization;
using System.Threading;

namespace Motor_Tracker_Configuration
{
    class Lib
    {
        public static byte[] ObjectToByteArray(object obj)
        {
            IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(obj));

            Marshal.StructureToPtr(obj, ptr, false);

            byte[] arr = new byte[Marshal.SizeOf(obj)];

            Marshal.Copy(ptr, arr, 0, arr.Length);

            Marshal.FreeHGlobal(ptr);

            return arr;
        }

        public static object ByteArrayToObject(byte[] buf, Type objectType)
        {
            IntPtr ptr = Marshal.AllocHGlobal(buf.Length);
            Marshal.Copy(buf, 0, ptr, buf.Length);

            object Obj = Marshal.PtrToStructure(ptr, objectType);

            Marshal.FreeHGlobal(ptr);

            return Obj;
        }

        public static string latlngToString(float value)
        {
            double degree = Math.Floor(value / 100);
            double min = value - degree * 100;

            return (degree + min / 60).ToString(".########", CultureInfo.InvariantCulture);
        }
    }

    /// <summary>
    /// Helper class for invoking tasks with timeout. Overhead is 0,005 ms.
    /// </summary>
    /// <typeparam name="TResult">The type of the result.</typeparam>

    public sealed class WaitFor<TResult>
    {
        readonly TimeSpan _timeout;

        /// <summary>
        /// Initializes a new instance of the <see cref="WaitFor{T}"/> class, 
        /// using the specified timeout for all operations.
        /// </summary>
        /// <param name="timeout">The timeout.</param>
        public WaitFor(TimeSpan timeout)
        {
            _timeout = timeout;
        }

        /// <summary>
        /// Executes the spcified function within the current thread, aborting it
        /// if it does not complete within the specified timeout interval. 
        /// </summary>
        /// <param name="function">The function.</param>
        /// <returns>result of the function</returns>
        /// <remarks>
        /// The performance trick is that we do not interrupt the current
        /// running thread. Instead, we just create a watcher that will sleep
        /// until the originating thread terminates or until the timeout is
        /// elapsed.
        /// </remarks>
        /// <exception cref="ArgumentNullException">if function is null</exception>
        /// <exception cref="TimeoutException">if the function does not finish in time </exception>
        public TResult Run(Func<TResult> function)
        {
            if (function == null)
            {
                throw new ArgumentNullException("function is null");
            }

            var sync = new object();
            var isCompleted = false;

            WaitCallback watcher = obj =>
            {
                var watchedThread = obj as Thread;

                lock (sync)
                {
                    if (!isCompleted)
                    {
                        Monitor.Wait(sync, _timeout);
                    }
                }
                // CAUTION: the call to Abort() can be blocking in rare situations
                // http://msdn.microsoft.com/en-us/library/ty8d3wta.aspx
                // Hence, it should not be called with the 'lock' as it could deadlock
                // with the 'finally' block below.

                if (!isCompleted)
                {
                    watchedThread.Abort();
                }
            };

            try
            {
                ThreadPool.QueueUserWorkItem(watcher, Thread.CurrentThread);
                return function();
            }
            catch (ThreadAbortException)
            {
                // This is our own exception.
                Thread.ResetAbort();

                throw new TimeoutException(string.Format("The operation has timed out after {0}.", _timeout));
            }
            finally
            {
                lock (sync)
                {
                    isCompleted = true;
                    Monitor.Pulse(sync);
                }
            }
        }

        /// <summary>
        /// Executes the spcified function within the current thread, aborting it
        /// if it does not complete within the specified timeout interval.
        /// </summary>
        /// <param name="timeout">The timeout.</param>
        /// <param name="function">The function.</param>
        /// <returns>result of the function</returns>
        /// <remarks>
        /// The performance trick is that we do not interrupt the current
        /// running thread. Instead, we just create a watcher that will sleep
        /// until the originating thread terminates or until the timeout is
        /// elapsed.
        /// </remarks>
        /// <exception cref="ArgumentNullException">if function is null</exception>
        /// <exception cref="TimeoutException">if the function does not finish in time </exception>
        public static TResult Run(TimeSpan timeout, Func<TResult> function)
        {
            return new WaitFor<TResult>(timeout).Run(function);
        }
    }

}

#include "ocilib_tests.h"

namespace TestCApi
{

    static Context context;

    static inline void AddError(OCI_Error* err)
    {
        context.Errs.emplace_back(Error{ OCI_ErrorGetInternalCode(err), OCI_ErrorGetOCICode(err) });
    }

    TEST(TestConnection, ValidCredentials)
    {
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);

        ASSERT_NE(nullptr, conn);

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestConnection, InvalidUser)
    {
        Guard guard(context.Lock);
        context.Errs.clear();

        ASSERT_TRUE(OCI_Initialize(AddError, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD_WRONG, OCI_SESSION_DEFAULT);

        ASSERT_EQ(nullptr, conn);

        ASSERT_EQ(1, context.Errs.size());
        ASSERT_EQ(0, context.Errs[0].LibCode);
        ASSERT_EQ(1017, context.Errs[0].OCICode);

        ASSERT_FALSE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestConnection, InvalidPassword)
    {
        Guard guard(context.Lock);
        context.Errs.clear();

        ASSERT_TRUE(OCI_Initialize(AddError, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD_WRONG, OCI_SESSION_DEFAULT);
        ASSERT_EQ(nullptr, conn);

        ASSERT_EQ(1, context.Errs.size());
        ASSERT_EQ(0, context.Errs[0].LibCode);
        ASSERT_EQ(1017, context.Errs[0].OCICode);

        ASSERT_FALSE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestConnection, Abort)
    {
        Guard guard(context.Lock);
        context.Errs.clear();

        ASSERT_TRUE(OCI_Initialize(AddError, HOME, OCI_ENV_DEFAULT | OCI_ENV_THREADED));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto thrd = OCI_ThreadCreate();
        ASSERT_NE(nullptr, thrd);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        auto callback = [](OCI_Thread* thread, void* data)
            {
                ASSERT_FALSE(OCI_ExecuteStmt(static_cast<OCI_Statement*>(data), OTEXT("begin dbms_lock.sleep(10); end;")));
            };

        OCI_ThreadRun(thrd, callback, stmt);

        Sleep(1000);
        ASSERT_TRUE(OCI_Break(conn));

        ASSERT_TRUE(OCI_ThreadJoin(thrd));
        ASSERT_TRUE(OCI_ThreadFree(thrd));

        ASSERT_EQ(1, context.Errs.size());
        ASSERT_EQ(0, context.Errs[0].LibCode);
        ASSERT_EQ(1013, context.Errs[0].OCICode);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_Cleanup());
    }
}